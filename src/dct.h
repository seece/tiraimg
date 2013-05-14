#ifndef DCT_H_
#define DCT_H_

#include "block.h"

void dct_calculate(struct ByteBlock const* input, 
		struct FloatBlock* output); 

#endif
