#ifndef IMAGE_H_
#define IMAGE_H

#include <stdint.h>

struct Image {
	int32_t width;
	int32_t height;
	char * data;
};
#endif

