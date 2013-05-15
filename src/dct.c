
#include <math.h>
#include <assert.h>
#include "eks_math.h"
#include "block.h"
#include "dct.h"

uint8_t const quantization_matrix[8][8] = {
	16, 11, 10, 16, 24, 40, 51, 61,
	12, 12, 14, 19, 26, 58, 60, 55,
	14, 13, 16, 24, 40, 57, 69, 56,
	14, 17, 22, 29, 51, 87, 80, 62,
	18, 22, 37, 56, 68, 109, 103, 77,
	24, 35, 55, 64, 81, 104, 113, 92,
	49, 64, 78, 87, 103, 121, 120, 101,
	72, 92, 95, 98, 112, 100, 103, 99
};

void quantize_floatblock(
		const struct FloatBlock* input,
		int32_t quality,
		struct ByteBlock* output)
{
	float mult;
	int32_t size = TIMG_BLOCK_SIZE;

	assert(quality > 0);
	assert(quality <= 100);

	if (quality > 50) {
		mult = (100.0-(float)quality)/50.0;
	} else {
		mult = 50.0/(float)quality;
	}

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			float in = (float)input->data[y][x];
			float scale = (float)quantization_matrix[y][x];
			int32_t out = round(in/(scale * mult));			
			output->data[y][x] = MIN(MAX(out, 0), 255);
		}
	}
}

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

void idct_calculate(struct ByteBlock const* dctdata,
		struct FloatBlock* output)
{
	int32_t size = TIMG_BLOCK_SIZE;
	//struct FloatBlock biased_input;
	//bias_block(input, -128.0, &biased_input);

	// Loop through pixels.
	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			float val = single_idct(x, y, dctdata);
			output->data[y][x] = val;
		}
	}
}
