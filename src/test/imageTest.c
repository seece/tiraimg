#include <stdio.h>
#include "CuTest.h"
#include "image.h"

void TestImageLoaderInit(CuTest* tc) 
{
	init_image_loader();

}

void TestLoadSmallImage(CuTest* tc) 
{
	struct Image* imagep = load_image("testdata/small.ppm");
	CuAssertIntEquals(tc, 32, imagep->width);
	CuAssertIntEquals(tc, 32, imagep->height);

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
	struct Image* imagep = load_image("testdata/small.ppm");
	struct BlockArray array;

	image_to_blockarray(imagep, &array);

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
	return suite;
}
