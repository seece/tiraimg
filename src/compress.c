#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "block.h"
#include "tiraimg.h"
#include "compress.h"
#include "jpeg.h"
#include "image/image.h"
#include "dct.h"
#include "huffman.h"

/**
 * @brief Size of an serialized ImageHeader.
 */
const int image_header_size = 4 + 2*sizeof(int32_t) + 1;

/**
 * @brief Header container for import/export operations.
 */
struct ImageHeader {
	char magic[4];
	int32_t width;
	int32_t height;
	uint8_t quality;
};

/**
 * @brief Calculates an in-place DCT for the given BlockArray, and saves
 * the quantized results.
 *
 * @param arrayp target array
 * @param quality compression quality, range: [1, 100]
 */
void compress_blockarray_dct(struct BlockArray* arrayp, int32_t quality)
{
	int32_t rows = arrayp->rows;
	int32_t cols = arrayp->columns;
	struct FloatBlock temp;
	struct FloatBlock quantized_coeffs;

	for (int y=0;y<rows;y++) {
		for (int x=0;x<cols;x++) {
			int32_t ofs = y*arrayp->columns + x;
			struct ColorBlock* cblock = &arrayp->data[ofs];

			for (int i=0;i<3;i++) {
				dct_calculate(&cblock->chan[i], &temp);
				dct_quantize_floatblock_float(&temp, quality, &quantized_coeffs);
				floatblock_add(&quantized_coeffs, 128.0, &quantized_coeffs);
				floatblock_to_byte(&quantized_coeffs, &cblock->chan[i]);

			}
		}
	}
}

/**
 * @brief Calculates an in-place inverse DCT for the given BlockArray. Decodes the
 * quantized values before processing.
 *
 * @param arrayp target BlockArray with quantized DCT-coefficients
 * @param quality the quality level used in compression
 */
void compress_blockarray_dct_inverse(struct BlockArray* arrayp, int32_t quality)
{
	int32_t rows = arrayp->rows;
	int32_t cols = arrayp->columns;
	struct FloatBlock temp;
	struct ByteBlock quant_matrix;
	struct FloatBlock float_quant_matrix;

	// We introduce some rounding error by not storing
	// the quantization matrix as an array of floats in 
	// the first place. 
	get_scaled_quant_matrix(quality, &quant_matrix);
	byteblock_to_float(&quant_matrix, &float_quant_matrix);

	for (int y=0;y<rows;y++) {
		for (int x=0;x<cols;x++) {

			int32_t ofs = y*arrayp->columns + x;
			struct ColorBlock* cblock = &arrayp->data[ofs];

			for (int i=0;i<3;i++) {
				byteblock_to_float(&cblock->chan[i], &temp);
				floatblock_add(&temp, -128.0, &temp);
				floatblock_multiply(&float_quant_matrix, &temp);

				dct_calculate_inverse(&temp, &cblock->chan[i]);
			}
		}
	}
}

/**
 * @brief Copies ByteBlock data to a given buffer, but discards
 * the trailing zeros. Also known as zero run length.
 *
 * @param block the source block
 * @param output destination byte buffer, must be at least 64 bytes 
 * long.
 * @return The amount of bytes written to the buffer.
 */
int32_t compress_block_encode(const struct ByteBlock* block, 
	uint8_t* output)
{
	assert(block);
	int32_t length = 64;

	for (int32_t i=63;i>=0;i--) {
		int32_t x = i % 8;
		int32_t y = i / 8;

		int32_t val = block->data[y][x];

		if (val != TIMG_ZERO_BYTE) {
			length = i+1;
			break;
		}
	}

	assert(length > 0);
	assert(length <= 64);
	memcpy(output, block->data, sizeof(uint8_t) * length);
	return length;
}


/**
 * @brief Decodes an ZRL (zero run length )compressed block from a 
 * data stream.
 *
 * @param input serialized block data
 * @param block output block
 *
 * @return the number of bytes read
 */
int32_t compress_block_decode(uint8_t* input, struct ByteBlock* block)
{
	int32_t pos = 0;
	int32_t length = 0;

	memset(block->data, TIMG_ZERO_BYTE, sizeof(block->data));
	memcpy(&length, input, 1); // we assume a little-endian architecture

	assert(length <= 64);

	pos+=1;

	for (int32_t i=0;i<length;i++) {
		int32_t x, y;
		x = i % 8;
		y = i / 8;

		block->data[y][x] = input[pos+i];
	}

	pos+=length;

	return pos;
}

/**
 * @brief Serializes the given BlockArray to the destination byte array.
 *
 * @param arrayp the array to serialize
 * @param dest serialization target, should have enough space to store
 * the results even in the worst case scenario.
 *
 * @return number of bytes written to the array
 */
static uint64_t write_block_data(struct BlockArray* arrayp, uint8_t* dest)
{
	struct ByteBlock tempblock;
	int32_t blocks = arrayp->columns * arrayp->rows;
	int32_t block_length;
	uint64_t written = 0;

	for (int i=0;i<blocks;i++) {
		struct ColorBlock* cblock = &arrayp->data[i];

		for (int u=0;u<3;u++) {
			byteblock_pack(&cblock->chan[u], &tempblock);
			// We save the block length + actual block data
			uint64_t block_start = written++;
			block_length = compress_block_encode(&cblock->chan[u], &dest[written]);
			assert(block_length<=64);
			memmove(dest + block_start, &block_length, 1);

			written += block_length;
		}
	}

	return written;
}

/**
 * @brief Unserializes a BlockArray from the given data stream.
 *
 * @param data source data
 * @param arrayp destination BlockArray, data field will be allocated
 *
 * @return number of bytes read
 */
static uint64_t read_block_data(uint8_t* data, struct BlockArray* arrayp)
{
	int32_t blocks = arrayp->columns * arrayp->rows;
	arrayp->data = calloc(blocks, sizeof(struct ColorBlock));

	size_t pos = 0;

	for (int32_t i=0;i<blocks;i++) {
		for (int32_t u=0;u<3;u++) {
			pos += compress_block_decode(data+pos, &arrayp->data[i].chan[u]);
		}
	}

	return pos;
}


/**
 * @brief Serializes an image file header.
 *
 * @param header source header struct
 * @param output target data array
 *
 * @return number of bytes written
 */
uint64_t serialize_header(struct ImageHeader* header, uint8_t* output)
{
	memmove(output, header->magic, 4);
	memmove((output+ 4), &header->width, 4);
	memmove((output+ 8), &header->height, 4);
	memmove((output+ 12), &header->quality, 1);

	return image_header_size;
}


/**
 * @brief Unserialize a header from a data stream.
 *
 * @param data source datat
 * @param header destination header
 *
 * @return number of bytes read
 */
uint64_t unserialize_header(uint8_t* data, struct ImageHeader* header)
{
	memcpy(&header->magic, data, 4);
	memcpy(&header->width, data+4, 4);
	memcpy(&header->height, data+8, 4);
	memcpy(&header->quality, data+12, 1);

	return image_header_size;
}

/**
 * @brief Compresses the given image and returns a binary buffer with
 * the saved image data.
 *
 * @param imagep source image
 * @param quality compression quality, range: [1, 100]
 * @param length compressed binary blob length is saved here
 *
 * @return pointer to the compressed image data
 */
uint8_t* compress_image_full(const struct Image* imagep, int32_t quality,
	uint64_t* length)
{
	struct Image* tempimage = image_clone(imagep);
	struct BlockArray array;

	assert(tempimage);

	image_to_ycbcr(tempimage);
	image_to_blockarray(tempimage, &array);
	compress_blockarray_dct(&array, quality);

	int32_t blocks = array.columns * array.rows;
	// magic + width & height + quality level (uint8_t) 
	int32_t header_length = image_header_size;
 	// pixel data + block length information for each channel + header
	uint64_t max_length = blocks*64*3 + blocks*3 + header_length; 
	uint8_t* temp = malloc(max_length);
	uint8_t* block_data = NULL;
	uint64_t datasize = 0;
	uint64_t block_data_length, image_data_length;

	struct ImageHeader header = {
		.magic = {'C', 'A', 'T', 'S'},
		.width = imagep->width,
		.height= imagep->height,
		.quality = quality & 0xFF,
	};

	datasize += serialize_header(&header, temp);

	image_data_length = write_block_data(&array, temp+datasize);

	assert(image_data_length > 0);
	block_data = huffman_encode(temp+datasize, image_data_length, &block_data_length);
	assert(block_data_length > 0);
	datasize += block_data_length;
	assert(header_length + block_data_length == datasize);
	
	uint8_t* finaldata = malloc(datasize);
	memcpy(finaldata, temp, header_length);
	memcpy(finaldata + header_length, block_data, block_data_length);
	*length = datasize;

	free(block_data);
	free(temp);
	blockarray_free(&array);
	image_del(tempimage);

	return finaldata;
}

/**
 * @brief Decompresses a loaded image.
 *
 * @param data the image data loaded from a file
 * @param length data length in bytes
 * @param flags decompression flags bit vector, see compress.h for flag values
 *
 * @return  pointer to the decompressed image
 */
struct Image* decompress_image_full(uint8_t* data, uint64_t length, uint32_t flags)
{
	uint64_t datasize = 0;
	uint64_t pos = 0;
	struct ImageHeader header;
	struct BlockArray array;

	pos += unserialize_header(data, &header);
	array.columns= header.width/8;
	array.rows = header.height/8;
	array.width = header.width;
	array.height = header.height;

	if (strncmp(tiraimg_magic, header.magic, 4)) {
		fprintf(stderr, "Invalid magic value!\n");
		return NULL;
	}

	printf("Dimensions: %dx%d, quality: %d\n", header.width, header.height, 
			header.quality);

	uint64_t data_len = length - pos;

	uint8_t* imagedata = huffman_decode(data + pos, data_len, &datasize);
	uint64_t real_datasize = read_block_data(imagedata, &array);

	assert(datasize == real_datasize);
	compress_blockarray_dct_inverse(&array, header.quality);

	struct Image* imagep = blockarray_to_image(&array);

	if (!(flags & COMPRESS_KEEP_YCBCR))
		image_to_rgb(imagep);

	free(imagedata);

	return imagep;
}
