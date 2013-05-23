/**
 * @file image.h
 * @brief Image processing datatypes and functions.
 */


#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>
#include <stdbool.h>
#include "block.h"
#include "pixel.h"

/**
 * @brief Represents an image made of pixels with three color channels.
 */
struct Image {
	int32_t width;
	int32_t height;
	struct Pixel* data;
};

/**
 * @brief An image made of ByteBlocks.
 */
struct BlockArray {
	/** width in blocks */
	int32_t columns;	
	/** height in blocks */
	int32_t rows;		
	/** actual image width in pixels */
	int32_t width;		
	/** actual image height in pixels */
	int32_t height;		

	/** array of ColorBlocks that hold the data */
	struct ColorBlock {
		/** color data with each channel in its respective block */
		struct ByteBlock chan[3];
	} * data;
};

bool init_image_loader(void);
void del_image(struct Image* imagep);
struct Image* new_image(int32_t width, int32_t height);
struct Image* load_image(const char * path);
struct Pixel image_read_pixel(struct Image* imagep, int32_t x, int32_t y);
struct Pixel blockarray_read_pixel(struct BlockArray* arrayp, int32_t x, int32_t y);
void image_to_blockarray(struct Image* imagep, struct BlockArray* arrayp);
void free_blockarray(struct BlockArray* arrayp);

void image_fill_noise(struct Image* imagep, int32_t seed);
int64_t image_save(const char * path, struct Image* imagep);
#endif

