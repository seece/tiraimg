#include <stdint.h>
#include <assert.h>
#include <math.h>
#include "../image/image.h"
#include "../eks_math.h"


/**
 * @brief Calculates the mean absolute error from the differences between
 * the two given images.
 *
 * @param expected baseline image
 * @param result the actual image to analyze
 *
 * @return the mean absolute error
 */
double stat_image_mean_absolute_error(struct Image* expected, struct Image* result)
{
	assert(expected);
	assert(result);
	assert(expected->width == result->width);
	assert(expected->height == result->height);
	
	int32_t count = expected->width * expected->height;
	double sum = 0.0;

	for (int32_t i=0;i<count;i++) {
		struct Pixel* exp = &expected->data[i];
		struct Pixel* res = &result->data[i];

		sum += abs(res->r - exp->r);
		sum += abs(res->g - exp->g);
		sum += abs(res->b - exp->b);
	}

	// We multiply the count by three since we consider each color channel
	// to be a single sample.
	return sum/(double)(count*3);
}
