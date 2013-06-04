#ifndef STAT_H_
#define STAT_H_

#include "image/image.h"

double stat_image_mean_absolute_error(struct Image* expected, struct Image* result);

#endif
