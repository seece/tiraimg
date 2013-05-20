#include "CuTest.h"
#include "image.h"

void TestImageLoaderInit(CuTest* tc) 
{
	init_image_loader();
}

CuSuite* CuGetImageSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestImageLoaderInit);
	return suite;
}
