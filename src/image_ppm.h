/**
 * @file image_ppm.h
 * @brief PPM-format image loading functions.
 */

#ifndef IMAGE_PPM_H_
#define IMAGE_PPM_H_
#include <stdint.h>

struct Image;
struct Pixel;

bool init_ppm_image_loader(void);
bool load_ppm_image(const char* path, int32_t* width, int32_t* height, struct Pixel** data);
int64_t image_ppm_save(const char * path, struct Image* imagep);
#endif
