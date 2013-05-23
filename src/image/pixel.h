#ifndef PIXEL_H_
#define PIXEL_H_
#include <stdint.h>


/**
 * @brief A single pixel with three color channels.
 */
struct Pixel {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

#endif
