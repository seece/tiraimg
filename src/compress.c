#include <stdlib.h>
#include <stdio.h>
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

				//byteblock_to_float(&cblock->chan[i], &temp);

				//floatblock_to_byte(&temp, &cblock->chan[i]);
				//dct_quantize_floatblock(&temp, quality, &cblock->chan[i]);
			}
		}
	}
}
