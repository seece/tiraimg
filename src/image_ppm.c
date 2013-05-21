#include <pam.h>
#include <ppm.h>
#include <stdbool.h>
#include "pixel.h"
#include "image_ppm.h"

/**
 * Initializes libnetpbm image loading library.
 */
bool init_ppm_image_loader(void)
{
	pm_init("tiraimg", 0);

	return false;
}

bool load_ppm_image(
	char* path, 
	int32_t* width, 
	int32_t* height, 
	struct Pixel** data)
{

	FILE* fp = pm_openr(path);
	int64_t amount;
	int32_t imagewidth;
	int32_t imageheight;
	pixval maxval;
	pixel** imagepixels;

	if (fp == NULL) {
		return true;
	}

	imagepixels = ppm_readppm(fp, &imagewidth, &imageheight, &maxval);

	pm_close(fp);

	int32_t pixelamount = imagewidth * imageheight;
	struct Pixel* finalpixels = malloc(sizeof(struct Pixel) * pixelamount);

	*width = imagewidth;
	*height= imageheight;

	for (int32_t y=0;y<imageheight;y++) {
		for (int32_t x=0;x<imagewidth;x++) {
			pixel* pix = &imagepixels[y][x];
			int32_t offset = y*imagewidth + x;

			(finalpixels + offset)->r = pix->r;
			(finalpixels + offset)->g = pix->g;
			(finalpixels + offset)->b = pix->b;
		}
	}

	*data = finalpixels;

	ppm_freearray(imagepixels, imageheight);

	return false;
}

