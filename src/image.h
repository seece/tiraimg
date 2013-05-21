#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>
#include <stdbool.h>
#include "block.h"
#include "pixel.h"


/**
 * Represents an image made of pixels with three color channels.
 */
struct Image {
	int32_t width;
	int32_t height;
	struct Pixel* data;
};


/**
 * An image made of ByteBlocks.
 */
struct BlockArray {
	int32_t columns;	// width in blocks
	int32_t rows;		// height in blocks
	int32_t width;		// actual image width in pixels
	int32_t height;		// actual image height in pixels

	struct ColorBlock {
		struct ByteBlock chan[3];
	} * data;
};

bool init_image_loader(void);
void del_image(struct Image* imagep);
struct Image* new_image(int32_t width, int32_t height);
struct Image* load_image(char * path);
void image_to_blockarray(struct Image* imagep, struct BlockArray* arrayp);
void free_blockarray(struct BlockArray* arrayp);

#endif

