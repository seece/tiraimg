#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <tgmath.h>
#include "pixel.h"
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
	img->data = malloc(datasize);
	memset(img->data, 0, datasize);

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
	struct Image* imagep = malloc(sizeof(struct Image));
	load_ppm_image(path, &imagep->width, &imagep->height, &imagep->data);
	return imagep;
}

void image_to_blockarray(struct Image* imagep, struct BlockArray* arrayp)
{
	const int size = TIMG_BLOCK_SIZE;

	assert(imagep && arrayp);
	assert(imagep->data);

	arrayp->width = imagep->width;
	arrayp->height = imagep->height;
	arrayp->columns = ceil(imagep->width/(double)size);
	arrayp->rows = ceil(imagep->height/(double)size);

	arrayp->data = NULL;
}


/**
 * @brief Frees all BlockArray data. Does not free the BlockArray itself.
 *
 * @param arrayp BlockArray whose data should be freed.
 */
void free_blockarray(struct BlockArray* arrayp)
{
	assert(arrayp);

	// TODO add a proper error message here?
	if (!arrayp->data)
		return

	free(arrayp->data);
}
