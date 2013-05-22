/**
 * @file image_ppm.h
 * @brief PPM-format image loading functions.
 */

#ifndef IMAGE_PPM_H_
#define IMAGE_PPM_H_
#include <stdint.h>

bool init_ppm_image_loader(void);
bool load_ppm_image(const char* path, int32_t* width, int32_t* height, struct Pixel** data);

#endif
