#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "block.h"
#include "jpeg.h"
#include "image/image.h"
#include "dct.h"

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

			//printf("%d, %d = %d\n", x, y, ofs);

			for (int i=0;i<3;i++) {
				//byteblock_add(&cblock->chan[i], -128, &temp);

				byteblock_to_float(&cblock->chan[i], &temp);
				floatblock_add(&temp, -128.0, &temp);
				floatblock_multiply(&float_quant_matrix, &temp);

				if ((y == x) && (y == 0) && (i == 0) ) {
					printf("topr:\n");
					byteblock_print(&cblock->chan[i]);
					printf("\n");
					//byteblock_print(&cblock->chan[i]);
					//byteblock_print(&quant_matrix);
					floatblock_print(&float_quant_matrix);
					printf("\n");
					floatblock_print(&temp);
					printf("\n");
				}

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

	memcpy(output, block->data, sizeof(uint8_t) * length);
	return length;
}

// returns a pointer to the allocated array
// this doesn't write any headers yet
uint8_t* compress_image_full(const struct Image* imagep, int32_t quality,
	uint64_t* length)
{
	struct Image* tempimage = image_clone(imagep);
	struct BlockArray array;
	assert(tempimage);

	image_to_ycbcr(tempimage);
	image_to_blockarray(tempimage, &array);

	// maximum of 65 bytes per MCU (
	int32_t blocks = imagep->width * imagep->height;
	// pixel data + length information (would cusotm huffman value be better??)
	uint64_t max_length = blocks*64*3 + blocks; 
	uint8_t* temp = malloc(max_length);

	uint64_t pos = 0;
	int32_t rows = array.rows;
	int32_t cols = array.columns;
	struct ByteBlock tempblock;

	for (int y=0;y<rows;y++) {
		for (int x=0;x<cols;x++) {
			int32_t ofs = y*array.columns + x;
			struct ColorBlock* cblock = &array.data[ofs];

			byteblock_pack(cblock, &tempblock);
			// TODO add serialization to temp array
			// TODO save image dimensions also somewhere

		}
	}

	free(temp);
	blockarray_free(&array);
	image_del(tempimage);

	return NULL;
}

