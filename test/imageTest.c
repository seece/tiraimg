#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CuTest.h"
#include "image/image.h"
#include "test_data.h"

void check_images_equal(CuTest* tc, struct Image* a, struct Image* b)
{
	CuAssertIntEquals(tc, a->width, b->width);
	CuAssertIntEquals(tc, a->height, b->height);

	int32_t size = a->width * a->height;

	for (int i=0;i<size;i++) {
		CuAssertIntEquals(tc, a->data[i].r, b->data[i].r);
		CuAssertIntEquals(tc, a->data[i].g, b->data[i].g);
		CuAssertIntEquals(tc, a->data[i].b, b->data[i].b);
	}

}

void TestImageLoaderInit(CuTest* tc) 
{
	image_init_loader();
}

void TestLoadSmallImage(CuTest* tc) 
{
	struct Image* imagep = image_load("testdata/tiny.ppm");
	CuAssertIntEquals(tc, 32, imagep->width);
	CuAssertIntEquals(tc, 16, imagep->height);

	int32_t amount = imagep->width * imagep->height;

	for (int i=0;i<amount;i++) {
		CuAssertIntEquals(tc, tinypic_data[i*3 + 0], imagep->data[i].r);
		CuAssertIntEquals(tc, tinypic_data[i*3 + 1], imagep->data[i].g);
		CuAssertIntEquals(tc, tinypic_data[i*3 + 2], imagep->data[i].b);
	}

	image_del(imagep);
}

void TestImageArraySize(CuTest* tc)
{
	struct Image* imagep = image_new(64, 65);
	struct BlockArray arrayp;

	image_to_blockarray(imagep, &arrayp);

	CuAssertIntEquals(tc, 64, arrayp.width);
	CuAssertIntEquals(tc, 65, arrayp.height);
	CuAssertIntEquals(tc, 8, arrayp.columns);
	CuAssertIntEquals(tc, 9, arrayp.rows);

	image_del(imagep);
	blockarray_free(&arrayp);
}

void TestCreatedImageSize(CuTest* tc)
{
	struct Image* imagep = image_new(64, 65);

	CuAssertIntEquals(tc, 64, imagep->width);
	CuAssertIntEquals(tc, 65, imagep->height);

	image_del(imagep);
}


/**
 * @brief Compares an Image to a BlockArray.
 *
 * @param tc the test case where to report assertion failures
 * @param imagep the image
 * @param arrayp the blockarray
 */
void compare_image_blockarray(CuTest* tc, struct Image* imagep, 
		struct BlockArray* arrayp)
{
	struct Pixel original;
	int32_t ofs, block_ofs;

	for (int y=0;y<imagep->height;y++) {
		for (int x=0;x<imagep->width;x++) {
			ofs = y*imagep->width + x;
			int32_t blockx = x/8;
			int32_t blocky = y/8;
			int32_t bx = x%8;
			int32_t by = y%8;

			block_ofs = (blockx) + ((blocky) * arrayp->columns);
			original = imagep->data[ofs];

			CuAssertIntEquals(tc, original.r, arrayp->data[block_ofs].chan[0].data[by][bx]);
			CuAssertIntEquals(tc, original.g, arrayp->data[block_ofs].chan[1].data[by][bx]);
			CuAssertIntEquals(tc, original.b, arrayp->data[block_ofs].chan[2].data[by][bx]);
		}
	}
}

void TestImageToBlocks(CuTest* tc)
{
	struct Image* imagep = image_load("testdata/small.ppm");
	struct BlockArray array;

	CuAssertTrue(tc, imagep != NULL);

	image_to_blockarray(imagep, &array);

	compare_image_blockarray(tc, imagep, &array);

	blockarray_free(&array);
	image_del(imagep);
}

void TestRandomImageToBlocks(CuTest* tc)
{
	struct Image* imagep = image_new(33, 32);
	struct BlockArray array;

	CuAssertTrue(tc, imagep != NULL);

	image_fill_noise(imagep, 12);
	image_to_blockarray(imagep, &array);

	compare_image_blockarray(tc, imagep, &array);

	blockarray_free(&array);
	image_del(imagep);
}

void TestImageReadPixel(CuTest* tc)
{
	struct Image* imagep = image_new(16, 20);

	imagep->data[0].r = 17;
	imagep->data[16*2 + 5].r = 10;
	imagep->data[16*10 + 15].g = 112;

	CuAssertIntEquals(tc, 17, image_read_pixel(imagep, 0, 0).r);
	CuAssertIntEquals(tc, 10, image_read_pixel(imagep, 5, 2).r);
	CuAssertIntEquals(tc, 112, image_read_pixel(imagep, 15, 10).g);

	image_del(imagep);
}

void TestImageReadPixelRandom(CuTest* tc)
{
	struct Image* imagep = image_new(65, 32);
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


	image_del(imagep);
}

void TestBlockArrayReadPixel(CuTest* tc)
{
	struct Image* imagep = image_new(16, 20);
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

			CuAssertIntEquals(tc, original.r, result.r);
			CuAssertIntEquals(tc, original.g, result.g);
			CuAssertIntEquals(tc, original.b, result.b);
		}
	}

	blockarray_free(&array);
	image_del(imagep);
}

void TestImageSave(CuTest* tc) 
{
	struct Image* imagep = image_load("testdata/tiny.ppm");
	struct Image* result_image;

	bool save_result = image_save("temp/testimage.ppm", imagep);
	CuAssertTrue(tc, save_result);

	result_image = image_load("temp/testimage.ppm");
	CuAssertTrue(tc, result_image != NULL);
	CuAssertTrue(tc, result_image->data != NULL);

	CuAssertIntEquals(tc, imagep->width, result_image->width);
	CuAssertIntEquals(tc, imagep->height, result_image->height);

	check_images_equal(tc, imagep, result_image);

	for (int i=0;i<imagep->width*imagep->height;i++) {
		CuAssertIntEquals(tc, imagep->data[i].r, result_image->data[i].r);
		CuAssertIntEquals(tc, imagep->data[i].g, result_image->data[i].g);
		CuAssertIntEquals(tc, imagep->data[i].b, result_image->data[i].b);
	}

	image_del(imagep);
	image_del(result_image);
}


void TestImageClone(CuTest* tc)
{
	struct Image* orig = image_load("testdata/tiny.ppm");
	struct Image* imagep = image_clone(orig);
	
	check_images_equal(tc, orig, imagep);

	image_del(imagep);
	image_del(orig);
}

void TestImageYCbCrConversion(CuTest* tc) 
{
	struct Image* orig = image_load("testdata/tiny.ppm");
	struct Image* imagep = image_clone(orig);

	image_to_ycbcr(imagep);
	image_to_rgb(imagep);

	check_images_equal(tc, orig, imagep);

	image_del(imagep);
	image_del(orig);
}

CuSuite* CuGetImageSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestImageLoaderInit);
	SUITE_ADD_TEST(suite, TestLoadSmallImage);
	SUITE_ADD_TEST(suite, TestImageArraySize);
	SUITE_ADD_TEST(suite, TestCreatedImageSize);
	SUITE_ADD_TEST(suite, TestImageToBlocks);
	SUITE_ADD_TEST(suite, TestRandomImageToBlocks);
	SUITE_ADD_TEST(suite, TestImageReadPixel);
	SUITE_ADD_TEST(suite, TestImageReadPixelRandom);
	SUITE_ADD_TEST(suite, TestBlockArrayReadPixel);
	SUITE_ADD_TEST(suite, TestImageSave);
	SUITE_ADD_TEST(suite, TestImageClone);
	SUITE_ADD_TEST(suite, TestImageYCbCrConversion);

	return suite;
}
