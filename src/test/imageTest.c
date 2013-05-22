#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CuTest.h"
#include "image.h"
#include "test_data.h"

void TestImageLoaderInit(CuTest* tc) 
{
	init_image_loader();
}

void TestLoadSmallImage(CuTest* tc) 
{
	struct Image* imagep = load_image("testdata/tiny.ppm");
	CuAssertIntEquals(tc, 32, imagep->width);
	CuAssertIntEquals(tc, 16, imagep->height);

	int32_t amount = imagep->width * imagep->height;

	for (int i=0;i<amount;i++) {
		CuAssertIntEquals(tc, tinypic_data[i*3 + 0], imagep->data[i].r);
		CuAssertIntEquals(tc, tinypic_data[i*3 + 1], imagep->data[i].g);
		CuAssertIntEquals(tc, tinypic_data[i*3 + 2], imagep->data[i].b);
	}

	del_image(imagep);
}

void TestImageArraySize(CuTest* tc)
{
	struct Image* imagep = new_image(64, 65);
	struct BlockArray arrayp;

	image_to_blockarray(imagep, &arrayp);

	CuAssertIntEquals(tc, 64, arrayp.width);
	CuAssertIntEquals(tc, 65, arrayp.height);
	CuAssertIntEquals(tc, 8, arrayp.columns);
	CuAssertIntEquals(tc, 9, arrayp.rows);

	del_image(imagep);
	free_blockarray(&arrayp);
}

void TestCreatedImageSize(CuTest* tc)
{
	struct Image* imagep = new_image(64, 65);

	CuAssertIntEquals(tc, 64, imagep->width);
	CuAssertIntEquals(tc, 65, imagep->height);

	del_image(imagep);
}

void TestImageToBlocks(CuTest* tc)
{
	struct Image* imagep = new_image(32, 32);
	struct BlockArray array;
	struct Pixel original;
	int32_t ofs, block_ofs;

	CuAssertTrue(tc, imagep != NULL);
	CuAssertIntEquals(tc, 32, imagep->width);
	CuAssertIntEquals(tc, 32, imagep->height);

	image_fill_noise(imagep, 12);
	image_to_blockarray(imagep, &array);

	for (int y=0;y<imagep->height;y++) {
		for (int x=0;x<imagep->width;x++) {

			ofs = y*imagep->width + x;
			block_ofs = (x/8) + (y/8) * array.columns;
			original = imagep->data[ofs];

			CuAssertIntEquals(tc, original.r, array.data[block_ofs].chan[0].data[y%8][x%8]);
		}
	}

	free_blockarray(&array);
	del_image(imagep);
}

void TestImageReadPixel(CuTest* tc)
{
	struct Image* imagep = new_image(16, 20);

	imagep->data[0].r = 17;
	imagep->data[16*2 + 5].r = 10;
	imagep->data[16*10 + 15].g = 112;

	CuAssertIntEquals(tc, 17, image_read_pixel(imagep, 0, 0).r);
	CuAssertIntEquals(tc, 10, image_read_pixel(imagep, 5, 2).r);
	CuAssertIntEquals(tc, 112, image_read_pixel(imagep, 15, 10).g);

	del_image(imagep);
}

void TestImageReadPixelRandom(CuTest* tc)
{
	struct Image* imagep = new_image(65, 32);
	image_fill_noise(imagep, 11);
	struct Pixel original, result;
	int32_t ofs;

	for (int y=0;y<imagep->height;y++) {
		for (int x=0;x<imagep->width;x++) {
			ofs = y*imagep->width + x;
			original = imagep->data[ofs];
			result = image_read_pixel(imagep, x, y);

			CuAssertIntEquals(tc, original.r, result.r);
			CuAssertIntEquals(tc, original.g, result.g);
			CuAssertIntEquals(tc, original.b, result.b);
		}
	}


	del_image(imagep);
}

void TestBlockArrayReadPixel(CuTest* tc)
{
	struct Image* imagep = new_image(16, 20);
	struct BlockArray array;
	struct Pixel original, result;

	image_fill_noise(imagep, 10);
	// We assume the image-blockarray conversion already works correctly.
	image_to_blockarray(imagep, &array);

	for (int y=0;y<imagep->height;y++) {
		for (int x=0;x<imagep->width;x++) {
			// We also assume image_read_pixel gives always
			// correct results.
			original = image_read_pixel(imagep, x, y);
			result = blockarray_read_pixel(&array, x, y);

			/*
			CuAssertIntEquals(tc, original.r, result.r);
			CuAssertIntEquals(tc, original.g, result.g);
			CuAssertIntEquals(tc, original.b, result.b);
			*/
			/*
			printf("%d, %d\n", x, y);
			assert(original.r==result.r);
			assert(original.g==result.g);
			assert(original.b==result.b);
			*/
		}
	}

	free_blockarray(&array);
	del_image(imagep);
}

CuSuite* CuGetImageSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestImageLoaderInit);
	SUITE_ADD_TEST(suite, TestLoadSmallImage);
	SUITE_ADD_TEST(suite, TestImageArraySize);
	SUITE_ADD_TEST(suite, TestCreatedImageSize);
	SUITE_ADD_TEST(suite, TestImageToBlocks);
	SUITE_ADD_TEST(suite, TestImageReadPixel);
	SUITE_ADD_TEST(suite, TestImageReadPixelRandom);
	SUITE_ADD_TEST(suite, TestBlockArrayReadPixel);
	return suite;
}
