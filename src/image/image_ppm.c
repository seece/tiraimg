#include <stdio.h>
#include <assert.h>
#include <pam.h>
#include <ppm.h>
#include <stdbool.h> // pm_config.h redefines bool  
#include "pixel.h"
#include "image_ppm.h"
#include "image.h"


/**
 * @brief Initializes libnetpbm image loading library.
 *
 * @return false on success, true on failure
 */
bool init_ppm_image_loader(void)
{
	pm_init("tiraimg", 0);

	return false;
}


/**
 * @brief Loads a PPM format picture, and saves the result as an Pixel array.
 *
 * @param path the image path.
 * @param width pointer to the resulting width variable.
 * @param height pointer to the resulting height variable.
 * @param data pointer to a Pixel array, image data will be saved here.
 *
 * @return false on success, true on failure
 */
bool load_ppm_image(
	const char* path, 
	int32_t* width, 
	int32_t* height, 
	struct Pixel** data)
{

	FILE* fp = pm_openr(path);
	int32_t imagewidth;
	int32_t imageheight;
	pixval maxval;
	pixel** imagepixels;

	if (!fp) 
		return true;

	imagepixels = ppm_readppm(fp, &imagewidth, &imageheight, &maxval);

	pm_close(fp);

	int32_t pixelamount = imagewidth * imageheight;
	struct Pixel* finalpixels = malloc(sizeof(struct Pixel) * pixelamount);

	if (!finalpixels) 
		return true;

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


/**
 * @brief Saves an image to disk as PPM.
 *
 * @param path target file path
 * @param imagep file to be saved
 *
 * @return the amount of bytes written, 0 on error
 */
int64_t image_ppm_save(const char * path, struct Image* imagep)
{
	assert(imagep);

	int64_t bytes_written = 0;
	int32_t pixels = imagep->width * imagep->height;

	FILE* fp = fopen(path, "w");

	if (!fp)
		return 0;

	fprintf(fp, "P3\n");
	fprintf(fp, "# tiraimg export\n");
	fprintf(fp, "%d %d\n", imagep->width, imagep->height);
	fprintf(fp, "%d\n", 255);

	for (int32_t i=0;i<pixels;i++) {
		struct Pixel* pix = &imagep->data[i];
		fprintf(fp, "%d %d %d", pix->r, pix->g, pix->b);
		fprintf(fp, "\n");
	}

	bytes_written = ftell(fp);
	fclose(fp);

	return bytes_written;
}
