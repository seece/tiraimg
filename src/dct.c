/**
 * DCT calculation functions, and some helpers.
 */

#include <math.h>
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
void quantize_floatblock(
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

			int32_t out = round(in/quant_matrix.data[y][x]);			
			output->data[y][x] = MIN(MAX(out, 0), 255);
		}
	}
}

/**
 * @brief Truncates the given ByteBlock values, see <quantize_floatblock>"()"
 *
 * @param input Original float values.
 * @param quality The quality factor between 1-100.
 * @param output A block to hold the resulting values.
 */
void quantize_byteblock(
		const struct ByteBlock* input,
		int32_t quality,
		struct ByteBlock* output)
{
	struct FloatBlock float_input;
	byteblock_to_float(input, &float_input);
	quantize_floatblock(&float_input, quality, output);
}

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
	bias_block(input, -128.0, &biased_input);

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
static float single_idct(
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

	return sum;
}


/**
 * @brief Calculates an inverse cosine transform (DCT-III) of a FloatBlock.
 *
 * @param dctdata Normalized (scaled) DCT coefficients.
 * @param output Sample output block.
 */
void idct_calculate(struct FloatBlock const* dctdata,
		struct FloatBlock* output)
{
	int32_t size = TIMG_BLOCK_SIZE;
	//struct FloatBlock biased_input;
	//bias_block(input, -128.0, &biased_input);

	// Loop through pixels.
	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			float val = single_idct(x, y, dctdata);
			//val = 1.0f;
			output->data[y][x] = val;
		}
	}
}
