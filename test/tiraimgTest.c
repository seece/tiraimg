
#include <stdint.h>
#include "CuTest.h"
#include "block.h"
#include "jpeg.h"
#include "image/image.h"
#include "compress.h"
#include "test_data.h"
#include "testHelpers.h"

void TestBlockArrayDCT(CuTest* tc) 
{
	struct Image* imagep = image_new(65, 32);
	struct BlockArray array;
	const int32_t quality = 100;

	image_fill_noise(imagep, 10);
	image_to_blockarray(imagep, &array);

	compress_blockarray_dct(&array, quality);
	compress_blockarray_dct_inverse(&array, quality);

	struct Image* result = blockarray_to_image(&array);

	CuAssertTrue(tc, result != NULL);

	check_images_equal(tc, imagep, result, 0);

	image_del(result);
	blockarray_free(&array);
	image_del(imagep);
}

CuSuite* CuGetSuite(void) 
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestBlockArrayDCT);
	return suite;
}
