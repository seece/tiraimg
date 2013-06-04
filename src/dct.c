/**
 * DCT calculation functions, and some helpers.
 */

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "eks_math.h"
#include "block.h"
#include "dct.h"
#include "jpeg.h"

/**
 * @brief Truncates the given FloatBlock values.
 *
 * @param input Original float values.
 * @param quality The quality factor between 1-100.
 * @param output A block to hold the resulting values.
 */
void dct_quantize_floatblock(
		const struct FloatBlock* input,
		int32_t quality,
		struct ByteBlock* output)
{
	int32_t size = TIMG_BLOCK_SIZE;
	struct ByteBlock quant_matrix;

	assert(quality > 0);
	assert(quality <= 100);

	get_scaled_quant_matrix(quality, &quant_matrix);

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			float in = (float)input->data[y][x];

			int32_t out = round(in/(float)quant_matrix.data[y][x]);			
			output->data[y][x] = out;
		}
	}
}

/**
 * @brief Truncates the given ByteBlock values, see <dct_quantize_floatblock>"()"
 *
 * @param input Original float values.
 * @param quality The quality factor between 1-100.
 * @param output A block to hold the resulting values.
 */
void dct_quantize_byteblock(
		const struct ByteBlock* input,
		int32_t quality,
		struct ByteBlock* output)
{
	struct FloatBlock float_input;
	byteblock_to_float(input, &float_input);
	dct_quantize_floatblock(&float_input, quality, output);
}


/**
 * @brief Returns a normalized scale factor matching the given frequency. 
 *
 * @param frequency Spatial frequency.
 *
 * @return The normalized scale factor.
 */
static double normalize_scale_factor(int32_t frequency) 
{
	if (frequency == 0) 
		return sqrt(1.0/(double)TIMG_BLOCK_SIZE);

	return sqrt(2.0/(double)TIMG_BLOCK_SIZE);
}

/**
 * @brief Calculates a DCT-coefficient for a sample block.
 *
 * @param u The horizontal spatial frequency, range: [0, 7]
 * @param v The vertical spatial frequency, range: [0, 7]
 * @param input The input sample block.
 *
 * @return The calculated coefficient.
 */
static float single_dct(
		int32_t u, 
		int32_t v, 
		struct FloatBlock const* input)
{
	int32_t size = TIMG_BLOCK_SIZE;
	double phasemult = M_PI/(double)size;
	double scaleu = normalize_scale_factor(u);
	double scalev = normalize_scale_factor(v);
	double du = (double)u;
	double dv = (double)v;
	float sum = 0.0f;

	// We investigate each pixel to find the crosscorrelation
	// of the sample block on this frequency.
	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			float sample = input->data[y][x];
			float result;
			result =  cos(phasemult*(x+0.5)*du);	
			result *= cos(phasemult*(y+0.5)*dv);
			result *= scaleu * scalev * sample;
			sum += result;
		}
	}

	return sum;
}


/**
 * @brief Calculates a discrete cosine transform (DCT-II) of a block.
 *
 * @param input Block of unnormalized samples (pixels).
 * @param output Stores the DCT coefficients here.
 */
void dct_calculate(struct ByteBlock const* input,
		struct FloatBlock* output) 
{
	int32_t size = TIMG_BLOCK_SIZE;
	struct FloatBlock biased_input;
	byteblock_add(input, -128.0, &biased_input);

	// Loop through frequencies.
	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			float val = single_dct(x, y, &biased_input);
			output->data[y][x] = val;
		}
	}
}

/**
 * @brief Calculates a single sample from the given coefficients with IDCT.
 *
 * @param x The x-coordinate of the sample point, range: [0, 7]
 * @param y The y-coordinate of the sample point, range: [0, 7]
 * @param dctdata The DCT-coefficients to use.
 *
 * @return the sample value
 */
static int32_t single_idct(
		int32_t x, 
		int32_t y, 
		struct FloatBlock const* dctdata)
{
	int32_t size = TIMG_BLOCK_SIZE;
	double phasemult = M_PI/(double)size;
	//double scaleu = normalize_scale_factor(u);
	//double scalev = normalize_scale_factor(v);
	float sum = 0.0f;

	for (int32_t v=0;v<size;v++) {
		for (int32_t u=0;u<size;u++) {
			float sample = dctdata->data[v][u];
			float result;
			result = cos(phasemult*((double)x+0.5)*(double)u);	
			result *= cos(phasemult*((double)y+0.5)*(double)v);
			result *= normalize_scale_factor(u);
			result *= normalize_scale_factor(v);

			result*=sample;
			sum += result;
		}
	}

	return round(sum);
}


/**
 * @brief Calculates an inverse cosine transform (DCT-III) of a FloatBlock.
 *
 * @param dctdata Normalized (scaled) DCT coefficients.
 * @param output Sample output block.
 */
void dct_calculate_inverse(struct FloatBlock const* dctdata,
		struct ByteBlock* output)
{
	int32_t size = TIMG_BLOCK_SIZE;
	
	// Loop through pixels.
	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			int32_t val = single_idct(x, y, dctdata);
			// The sample values were biased by 128 before storage.
			output->data[y][x] = MIN(255, MAX(0, val + 128));
		}
	}
}

typedef void (*blockarray_map_func_t)(
		struct BlockArray* arrayp, 
		struct ColorBlock* blockp, 
		int32_t x, 
		int32_t y);

/*
static void blockarray_map(struct BlockArray* arrayp, blockarray_map_func_t func)
{
	int32_t rows = arrayp->rows;
	int32_t cols = arrayp->columns;
	struct FloatBlock temp;

	for (int y=0;y<rows;y++) {
		for (int x=0;x<cols;x++) {
			int32_t ofs = y*arrayp->columns + x;
			func(arrayp, &arrayp->data[ofs], x, y);
		}
	}
}
*/

/*
// not used since quality-parameter can't be passed here
static dct_blockarray_mapfunc(
		struct BlockArray* arrayp, 
		struct ColorBlock* blockp, 
		int32_t x, 
		int32_t y)
{
	struct FloatBlock temp;
	
	for (int i=0;i<3;i++) {
		dct_calculate(&blockp->chan[i], &temp);
		dct_quantize_floatblock(&temp, quality, &blockp->chan[i]);
	}
}
*/

void dct_quantize_floatblock_float(
		const struct FloatBlock* input,
		int32_t quality,
		struct FloatBlock* output)
{
	int32_t size = TIMG_BLOCK_SIZE;
	struct ByteBlock quant_matrix;

	assert(quality > 0);
	assert(quality <= 100);

	get_scaled_quant_matrix(quality, &quant_matrix);

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			float in = (float)input->data[y][x];

			float out = round(in/(float)quant_matrix.data[y][x]);			

			if (out > 127) {
				fprintf(stderr, "DCT overflow %f at (%d, %d)\n", out, x, y);
			}
			output->data[y][x] = MIN(127.0f, MAX(-127.0f, out));
		}
	}
}

