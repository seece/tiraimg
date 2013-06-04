
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CuTest.h"
#include "block.h"
#include "dct.h"
#include "jpeg.h"
#include "image/image.h"
#include "compress.h"
#include "stat/stat.h"
#include "test_data.h"
#include "testHelpers.h"

void TestBlockArrayDCT(CuTest* tc) 
{
	struct Image* imagep = image_load("testdata/tiny.ppm");
	struct BlockArray array;
	const int32_t quality = 80;

	image_to_ycbcr(imagep);
	image_to_blockarray(imagep, &array);

	compress_blockarray_dct(&array, quality);
	compress_blockarray_dct_inverse(&array, quality);

	struct Image* result = blockarray_to_image(&array);

	image_to_rgb(result);

	CuAssertTrue(tc, result != NULL);

	//image_save("temp/dctpic.ppm", result);
	double error = stat_image_mean_absolute_error(imagep, result);

	CuAssertTrue(tc, error < 40.0);
	// Equality checks are not useful for us, since the compression
	// can introduce single pixel artifacts even though rest of
	// the picture matches quite well.
	//check_images_equal(tc, imagep, result, 10);

	image_del(result);
	blockarray_free(&array);
	image_del(imagep);
}

void TestBlockEncode(CuTest* tc)
{
	struct ByteBlock output;
	struct ByteBlock quantized;
	uint8_t data[64];
	int32_t length = -1;
	int32_t quality = 25;

	dct_quantize_byteblock(&test_input, quality, &quantized);
	byteblock_pack(&quantized, &output);

	length = compress_block_encode(&output, data);

	CuAssertTrue(tc, length > 0);

	// Check that the data was copied successfully.
	for (int32_t i=0;i<length;i++) {
		int32_t x = i % 8;
		int32_t y = i / 8;
		CuAssertIntEquals(tc, data[i], output.data[y][x]);
	}
}

void TestImageCompression(CuTest* tc)
{
	struct Image* imagep = image_load("testdata/small.ppm");
	uint8_t* data = NULL;
	uint64_t length = 0;
	data = compress_image_full(imagep, 85, &length);
	CuAssertTrue(tc, data != NULL);
	CuAssertTrue(tc, length > 0);

	image_del(imagep);
}

CuSuite* CuGetSuite(void) 
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestBlockArrayDCT);
	SUITE_ADD_TEST(suite, TestBlockEncode);
	SUITE_ADD_TEST(suite, TestImageCompression);
	return suite;
}
