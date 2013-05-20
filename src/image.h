#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>
#include <stdbool.h>
#include "pixel.h"

struct Image {
	int32_t width;
	int32_t height;
	struct Pixel * data;
};

extern bool init_image_loader(void);
extern void del_image(struct Image* imagep);
extern struct Image* new_image(int32_t width, int32_t height);
extern struct Image* load_image(char * path);

#endif

