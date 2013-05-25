#include <stdlib.h>
#include <assert.h>
#include "block.h"
#include "image/image.h"
#include "dct.h"

void compress_blockarray_dct(struct BlockArray* arrayp, int32_t quality)
{
	int32_t rows = arrayp->rows;
	int32_t cols = arrayp->columns;
	struct FloatBlock temp;

	for (int y=0;y<rows;y++) {
		for (int x=0;x<cols;x++) {
			int32_t ofs = y*arrayp->columns + x;
			struct ColorBlock* blockp = &arrayp->data[ofs];

			for (int i=0;i<3;i++) {
				dct_calculate(&blockp->chan[i], &temp);
				dct_quantize_floatblock(&temp, quality, &blockp->chan[i]);
			}
		}
	}
}

void compress_blockarray_dct_inverse(struct BlockArray* arrayp, int32_t quality)
{
	int32_t rows = arrayp->rows;
	int32_t cols = arrayp->columns;
	struct FloatBlock temp;

	for (int y=0;y<rows;y++) {
		for (int x=0;x<cols;x++) {
			int32_t ofs = y*arrayp->columns + x;
			struct ColorBlock* blockp = &arrayp->data[ofs];

			for (int i=0;i<3;i++) {
				byteblock_to_float(&blockp->chan[i], &temp);
				//dct_calculate_inverse();
				//dct_quantize_floatblock(&temp, quality, &blockp->chan[i]);
			}
		}
	}
}
