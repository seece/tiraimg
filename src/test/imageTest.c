#include <stdio.h>
#include "CuTest.h"
#include "image.h"

void TestImageLoaderInit(CuTest* tc) 
{
	init_image_loader();

}

void TestLoadSmallImage(CuTest* tc) 
{
	//init_image_loader();
	struct Image* imagep = load_image("testdata/small.ppm");
}

CuSuite* CuGetImageSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestImageLoaderInit);
	SUITE_ADD_TEST(suite, TestLoadSmallImage);
	return suite;
}
