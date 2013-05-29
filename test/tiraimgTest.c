
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
#include "test_data.h"
#include "testHelpers.h"

void TestBlockArrayDCT(CuTest* tc) 
{
	struct Image* imagep = image_load("testdata/tiny.ppm");
	struct BlockArray array;
	const int32_t quality = 90;

	image_to_ycbcr(imagep);
	image_to_blockarray(imagep, &array);

	compress_blockarray_dct(&array, quality);
	compress_blockarray_dct_inverse(&array, quality);

	struct Image* result = blockarray_to_image(&array);

	image_to_rgb(result);

	CuAssertTrue(tc, result != NULL);

	//image_save("temp/dctpic.ppm", result);
	check_images_equal(tc, imagep, result, 5);

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

CuSuite* CuGetSuite(void) 
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestBlockArrayDCT);
	SUITE_ADD_TEST(suite, TestBlockEncode);
	return suite;
}
