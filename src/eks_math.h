/**
 * @file eks_math.h
 * @brief Additional math definitions that are not available in the C99 standard 
 * library.
 */

#ifndef EKS_MATH_H_
#define EKS_MATH_H_

#include <stdint.h>

#define M_PI 3.14159265358979323846

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int32_t abs(int32_t i);

#endif
