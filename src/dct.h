#ifndef DCT_H_
#define DCT_H_

#include "block.h"
#include "image/image.h"

void dct_calculate(struct ByteBlock const* input, 
		struct FloatBlock* output); 
void dct_calculate_inverse(struct FloatBlock const* dctdata,
		struct ByteBlock* output);

void dct_quantize_floatblock(
		const struct FloatBlock* input,
		int32_t quality,
		struct ByteBlock* output);

void dct_quantize_byteblock(
		const struct ByteBlock* input,
		int32_t quality,
		struct ByteBlock* output);

#endif
