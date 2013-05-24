#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <tgmath.h>
#include "pixel.h"
#include "../block.h"
#include "image.h"
#include "image_ppm.h"

typedef void (*image_map_func_t)(struct Pixel* pixelp, int32_t x, int32_t y);

/**
 * @brief Initializes image loading library.
 *
 * @return false on success, true on failure
 */
bool image_init_loader(void)
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
struct Image* image_new(int32_t width, int32_t height)
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
void image_del(struct Image* imagep)
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
struct Image* image_load(const char * path)
{
	struct Image* imagep = malloc(sizeof(struct Image));
	load_ppm_image(path, &imagep->width, &imagep->height, &imagep->data);
	return imagep;
}


/**
 * @brief Checks if the given point is inside a rectangle defined by w and h.
 *
 * @param x point x-coordinate 
 * @param y point y-coordinate
 * @param w rectangle width 
 * @param h rectangle height
 *
 * @return true if inside, otherwise false
 */
inline bool inside_bounds(int32_t x, int32_t y, int32_t w, int32_t h)
{
	if (x < 0 || y < 0)
		return false;

	if (x >= w || y >= h)
		return false;

	return true;
}


/**
 * @brief Reads a single pixel from the source image.
 * Invalid picture pointer or out-of-bounds coordinates will 
 * halt the program.
 *
 * @param imagep the target image
 * @param x pixel x-coordinate
 * @param y pixel y-coordinate
 *
 * @return pixel struct
 */
struct Pixel image_read_pixel(struct Image* imagep, int32_t x, int32_t y)
{
	assert(imagep);
	assert(imagep->data);
	assert(inside_bounds(x, y, imagep->width, imagep->height));

	int32_t ofs = y*imagep->width + x;
	return imagep->data[ofs];
}


/**
 * @brief Reads a single pixel from the source BlockArray.
 *
 * @param arrayp the target image as BlockArray
 * @param x pixel x-coordinate
 * @param y pixel y-coordinate
 *
 * @return the read Pixel struct 
 */
struct Pixel blockarray_read_pixel(struct BlockArray* arrayp, int32_t x, int32_t y)
{
	assert(arrayp);
	assert(arrayp->data);
	assert(inside_bounds(x, y, arrayp->width, arrayp->height));

	const int32_t size = TIMG_BLOCK_SIZE;

	int32_t column = x/size;
	int32_t row = y/size;
	int32_t ofs = row*arrayp->columns + column;
	int32_t block_x = x % size;
	int32_t block_y = y % size;

	struct ColorBlock* cblockp = &arrayp->data[ofs];

	struct Pixel p;
	p.r = cblockp->chan[0].data[block_y][block_x];
	p.g = cblockp->chan[1].data[block_y][block_x];
	p.b = cblockp->chan[2].data[block_y][block_x];

	return p;
}

/**
 * @brief Copies values from part of an image to a single ColorBlock.
 *
 * @param imagep
 * @param block_x
 * @param block_y
 * @param cblock the target ColorBlock
 */
static void copy_block(
		struct Image* imagep, 
		int32_t block_x, 
		int32_t block_y,
		struct ColorBlock* cblock) 
{
	const int32_t size = TIMG_BLOCK_SIZE;
	int32_t block_pixel_x = block_x * size;
	int32_t block_pixel_y = block_y * size;

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			int32_t px = block_pixel_x + x;	
			int32_t py = block_pixel_y + y;	
			int32_t p_offset = py*imagep->width + px;

			if (!inside_bounds(px, py, imagep->width, imagep->height)) 
				continue;

			struct Pixel p = imagep->data[p_offset];

			cblock->chan[0].data[y][x] = p.r;
			cblock->chan[1].data[y][x] = p.g;
			cblock->chan[2].data[y][x] = p.b;
		}
	}
}

/**
 * @brief Converts an Image to a BlockArray. Allocates memory to hold all 
 * color information.
 *
 * @param imagep the source image
 * @param arrayp result will be saved here
 */
void image_to_blockarray(struct Image* imagep, struct BlockArray* arrayp)
{
	const int32_t size = TIMG_BLOCK_SIZE;

	assert(imagep && arrayp);
	assert(imagep->data);
	assert(imagep->width > 0);
	assert(imagep->height > 0);

	arrayp->width = imagep->width;
	arrayp->height = imagep->height;

	int32_t cols = imagep->width/8;
	int32_t rows = imagep->height/8;

	if (imagep->width % size != 0)
		cols++;

	if (imagep->height % size != 0)
		rows++;

	int32_t amount = cols*rows;
	arrayp->columns = cols;
	arrayp->rows = rows;

	// calloc initializes all values to zero, which we like.
	arrayp->data = calloc(amount, sizeof(struct ColorBlock));

	// Loop through all the allocated blocks.
	for (int y=0;y<rows;y++) {
		for (int x=0;x<cols;x++) {
			int32_t ofs = y*arrayp->columns + x;
			struct ColorBlock* blockp = &arrayp->data[ofs];
			copy_block(imagep, x, y, blockp);
		}
	}

}


/**
 * @brief Frees all BlockArray data. Does not free the BlockArray itself.
 *
 * @param arrayp BlockArray whose data should be freed.
 */
void blockarray_free(struct BlockArray* arrayp)
{
	assert(arrayp);

	if (!arrayp->data) {
		fprintf(stderr, "Error: trying to free empty BlockArray!\n");
		return;
	}

	free(arrayp->data);
}


/**
 * @brief Fills the given image with random noise.
 *
 * @param imagep the target image
 * @param seed seed for the random generator
 */
void image_fill_noise(struct Image* imagep, int32_t seed)
{
	assert(imagep);
	srand(seed);
	int32_t imagesize = imagep->width*imagep->height;

	for (int i=0;i<imagesize;i++) {
		imagep->data[i].r = rand() % 256;
		imagep->data[i].g = rand() % 256;
		imagep->data[i].b = rand() % 256;
	}
}

/**
 * @brief Saves an image to disk. Currently the only format supported
 * is PPM.
 *
 * @param path target file path
 * @param imagep file to be saved
 *
 * @return the amount of bytes written, 0 on error
 */
int64_t image_save(const char * path, struct Image* imagep)
{
	return image_ppm_save(path, imagep);
}

static void image_map(struct Image* imagep, image_map_func_t func) {
	int32_t ofs;
	struct Pixel* pix;

	for (int32_t y=0;y<imagep->height;y++) {
		for (int32_t x=0;x<imagep->width;x++) {
			ofs = imagep->width*y + x;	
			pix = &imagep->data[ofs];
			func(pix, x, y);
		}
	}
}

static void rgb_to_ycbcr_mapfunc(struct Pixel* pixelp, int32_t x, int32_t y) 
{
	// We assume a range of [0, 255]
	float r = pixelp->r;
	float g = pixelp->g;
	float b = pixelp->b;

	// Y'
	pixelp->r = 0.0 + 0.299 *      r + 0.587 *    g + 0.114 *   b;
	// Cb
	pixelp->g = 128.0 - 0.168736 * r - 0.331264 * g + 0.5 *     b;
	// Cr
	pixelp->b = 128.0 + 0.5 *      r + 0.418688 * g - 0.081312 *b;
}

static void ycbcr_to_rgb_mapfunc(struct Pixel* pixelp, int32_t x, int32_t y) 
{
	float yy = pixelp->r;
	float cb = pixelp->g;
	float cr = pixelp->b;

	// red
	pixelp->r = yy                          + 1.402   * (cr - 128.0);
	// green
	pixelp->g = yy - 0.34414 * (cb - 128.0) - 0.71414 * (cr - 128.0);
	// blue
	pixelp->b = yy + 1.772   * (cb - 128.0);
}

void image_to_ycbcr(struct Image* imagep) {
	image_map(imagep, rgb_to_ycbcr_mapfunc);
}

void image_to_rgb(struct Image* imagep) {
	image_map(imagep, ycbcr_to_rgb_mapfunc);
}
