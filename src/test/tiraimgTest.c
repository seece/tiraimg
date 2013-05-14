
#include "CuTest.h"

void TestTrivial(CuTest* tc) 
{
	CuAssertTrue(tc, 1==1);
}

CuSuite* CuGetSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestTrivial);
	return suite;
}
