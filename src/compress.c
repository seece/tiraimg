#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "block.h"
#include "tiraimg.h"
#include "jpeg.h"
#include "image/image.h"
#include "dct.h"

/**
 * @brief Calculates an in-place DCT for the given BlockArray, and saves
 * the quantized results.
 *
 * @param arrayp target array
 * @param quality compresion quality, range: [1, 100]
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
 * the trailing zeros.   
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

		if (val != 0) {
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
	struct ByteBlock tempblock;
	assert(tempimage);

	image_to_ycbcr(tempimage);
	image_to_blockarray(tempimage, &array);

	int32_t blocks = array.columns * array.rows;
	// magic + width & height + quality level (uint8_t) 
	int32_t header_length = 4 + 2*sizeof(int32_t) + 1;
 	// pixel data + block length information for each channel + header
	uint64_t max_length = blocks*64*3 + blocks*3 + header_length; 
	uint8_t* temp = malloc(max_length);
	uint64_t written = 0;
	uint8_t quality_byte = quality & 0xFF;

	memmove(temp, tiraimg_magic, 4);
	memmove((temp + 4), &imagep->width, 4);
	memmove((temp + 8), &imagep->height, 4);
	memmove((temp + 12), &quality_byte, 1);

	written += header_length;

	int32_t block_length;

	for (int i=0;i<blocks;i++) {
		struct ColorBlock* cblock = &array.data[i];

		for (int u=0;u<3;u++) {
			byteblock_pack(&cblock->chan[u], &tempblock);
			// We save the block length + actual block data
			uint64_t block_start = written++;
			block_length = compress_block_encode(&cblock->chan[u], &temp[written]);
			assert(block_length<=64);
			memmove(temp + block_start, &block_length, 1);

			written += block_length;
		}
	}

	uint8_t* finaldata = malloc(written);
	memcpy(finaldata, temp, written);
	*length = written;

	free(temp);
	blockarray_free(&array);
	image_del(tempimage);

	return finaldata;
}

struct Image* decompress_image_full(uint8_t* data, uint64_t length)
{
	// TODO implement decompression
	return NULL;
}
