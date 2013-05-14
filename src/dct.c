
#include <math.h>
#include "eks_math.h"
#include "block.h"
#include "dct.h"

static double normalize_scale_factor(int frequency) 
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
	int size = TIMG_BLOCK_SIZE;
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
