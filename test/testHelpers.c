#include "testHelpers.h"


void byteblock_compare_test(CuTest* tc, const struct ByteBlock* a, const struct ByteBlock* b) 
{
	int size = TIMG_BLOCK_SIZE;

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertIntEquals(tc, a->data[y][x], b->data[y][x]);
		}
	}
}

void check_images_equal(CuTest* tc, struct Image* a, struct Image* b, int32_t delta)
{
	CuAssertIntEquals(tc, a->width, b->width);
	CuAssertIntEquals(tc, a->height, b->height);

	int32_t size = a->width * a->height;

	for (int i=0;i<size;i++) {
		if (delta == 0) {
			CuAssertIntEquals(tc, a->data[i].r, b->data[i].r);
			CuAssertIntEquals(tc, a->data[i].g, b->data[i].g);
			CuAssertIntEquals(tc, a->data[i].b, b->data[i].b);
		} else {
			CuAssertDblEquals(tc, a->data[i].r, b->data[i].r, delta);
			CuAssertDblEquals(tc, a->data[i].g, b->data[i].g, delta);
			CuAssertDblEquals(tc, a->data[i].b, b->data[i].b, delta);
		}
	}

}
