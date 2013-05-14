#include <stdio.h>
#include "CuTest.h"
//#include "

CuSuite* CuGetSuite();
CuSuite* CuGetDCTSuite();

void RunAllTests(void) 
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	CuSuiteAddSuite(suite, CuGetSuite());
	CuSuiteAddSuite(suite, CuGetDCTSuite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
	CuSuiteDelete(suite);
}

int main(void) 
{
	RunAllTests();

	return 0;
}

