/**
 * Strictly JPEG related constants and functions.
 */

#include <stdint.h>
#include <assert.h>
#include <math.h>
#include "eks_math.h"
#include "block.h"
#include "jpeg.h"

/**
 * @brief The official JPEG quantization matrix.
 */
uint8_t const quantization_matrix[8][8] = {
	{16, 11, 10, 16, 24, 40, 51, 61},
	{12, 12, 14, 19, 26, 58, 60, 55},
	{14, 13, 16, 24, 40, 57, 69, 56},
	{14, 17, 22, 29, 51, 87, 80, 62},
	{18, 22, 37, 56, 68, 109, 103, 77},
	{24, 35, 55, 64, 81, 104, 113, 92},
	{49, 64, 78, 87, 103, 121, 120, 101},
	{72, 92, 95, 98, 112, 100, 103, 99}
};


/**
 * @brief Calculates a scaled JPEG quantization matrix.
 *
 * @param quality The quality value to use, range: [1, 100]
 * @param output Pointer to the result ByteBlock.
 */
void get_scaled_quant_matrix(int32_t quality, struct ByteBlock* output)
{
	float mult;

	assert(quality > 0);
	assert(quality <= 100);

	if (quality > 50) {
		mult = (100.0-(float)quality)/50.0;
	} else {
		mult = 50.0/(float)quality;
	}

	for (int32_t y=0;y<8;y++) {
		for (int32_t x=0;x<8;x++) {
			int32_t in = quantization_matrix[y][x];
			int32_t out = round(in*mult);			

			// We do not want to save zeroes, since it would mean
			// we might have to divide by them later on!
			output->data[y][x] = MAX(1, out);
		}
	}
}
