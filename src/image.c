#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <tgmath.h>
#include "pixel.h"
#include "block.h"
#include "image.h"
#include "image_ppm.h"



/**
 * @brief Initializes image loading library.
 *
 * @return false on success, true on failure
 */
bool init_image_loader(void)
{
	return init_ppm_image_loader();
}


/**
 * @brief Creates a new image with the given dimensions.
 *
 * @param width image width in pixels
 * @param height image height in pixels
 *
 * @return pointer to the allocated Image structure.
 */
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


/**
 * @brief Deletes an allocated image.
 * Both the image data and the image struct are freed.
 *
 * @param imagep the image to be deleted
 */
void del_image(struct Image* imagep)
{
	assert(imagep);

	if (imagep->data != NULL) {
		free(imagep->data);	
	}

	free(imagep);
}


/**
 * @brief Loads an image from the disk.
 * Currently only PPM format is supported.
 *
 * @param path image file path
 *
 * @return pointer to the new image
 */
struct Image* load_image(const char * path)
{
	struct Image* imagep = malloc(sizeof(struct Image));
	load_ppm_image(path, &imagep->width, &imagep->height, &imagep->data);
	return imagep;
}

inline bool inside_bounds(int32_t x, int32_t y, int32_t w, int32_t h)
{
	if (x < 0 || y < 0)
		return false;

	if (x >= w || y >= h)
		return false;

	return true;
}

struct Pixel image_read_pixel(struct Image* imagep, int32_t x, int32_t y)
{
	assert(imagep);
	assert(imagep->data);
	assert(inside_bounds(x, y, imagep->width, imagep->height));

	int32_t ofs = y*imagep->width + x;
	return imagep->data[ofs];
}

static void fill_block(
		struct ColorBlock* cblock, 
		struct Image* imagep, 
		int32_t block_x, 
		int32_t block_y) 
{

	const int32_t size = TIMG_BLOCK_SIZE;

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			struct Pixel p;

			int32_t pixel_x = block_x + x;
			int32_t pixel_y = block_y + y;
			int32_t pixel_offset = (y)*imagep->width + x + 0;

			// The values outside picture area are all 0.
			if (inside_bounds(pixel_x, pixel_y, imagep->width, imagep->height)) {
				p = imagep->data[pixel_offset];
			} else {
				p.r = p.g = p.b = 0;
			}

			cblock->chan[0].data[y][x] = p.r;
			cblock->chan[1].data[y][x] = p.g;
			cblock->chan[2].data[y][x] = p.b;
			
		}
	}
}

void image_to_blockarray(struct Image* imagep, struct BlockArray* arrayp)
{
	const int32_t size = TIMG_BLOCK_SIZE;

	assert(imagep && arrayp);
	assert(imagep->data);
	assert(imagep->width > 0);
	assert(imagep->height > 0);

	arrayp->width = imagep->width;
	arrayp->height = imagep->height;

	// The division is rounded upwards.
	arrayp->columns = imagep->width/size + (imagep->width % size != 0);
	arrayp->rows = imagep->height/size + (imagep->height % size != 0);

	assert(arrayp->rows > 0);
	assert(arrayp->columns> 0);

	int32_t block_num = arrayp->columns * arrayp->rows;

	arrayp->data = calloc(block_num, sizeof(struct ColorBlock));

	for (int32_t y=0;y<arrayp->rows;y++) {
		for (int32_t x=0;x<arrayp->columns;x++) {
			int32_t ofs = y*arrayp->columns + x;
			fill_block(&arrayp->data[ofs], 
				imagep, x*size, y*size);
		}
	}

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
		return;

	free(arrayp->data);
}
