
#ifndef JPEG_H_
#define JPEG_H_
#include <stdint.h>

extern const uint8_t jpeg_quantization_matrix[8][8];
void jpeg_get_quant_matrix(int32_t quality, struct ByteBlock* output);

#endif
