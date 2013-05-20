#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include "image.h"
#include "image_ppm.h"

/**
 * Initializes image loading library.
 */
bool init_image_loader(void)
{
	return init_ppm_image_loader();
}

struct Image* new_image(int32_t width, int32_t height)
{
	struct Image* img = malloc(sizeof(struct Image));
	img->width = width;
	img->height = height;

	// Allocate space for three 8 bit channels.
	int32_t datasize = sizeof(uint8_t) * width * height * 3;
	memset(img, 0, datasize);

	return img;
}

void del_image(struct Image* imagep)
{
	assert(imagep);

	if (imagep->data != NULL) {
		free(imagep->data);	
	}

	free(imagep);
}

struct Image* load_image(char * path)
{
	return NULL;
}
