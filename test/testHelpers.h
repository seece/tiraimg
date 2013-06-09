#ifndef TEST_HELPERS_H_
#define TEST_HELPERS_H_

#include "CuTest.h"
#include "image/image.h"
#include "block.h"

void byteblock_compare_test(CuTest* tc, const struct ByteBlock* a, const struct ByteBlock* b) ;
void check_images_equal(CuTest* tc, struct Image* a, struct Image* b, int32_t delta);
#endif
