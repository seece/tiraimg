
#include <stdint.h>
#include "CuTest.h"
#include "block.h"
#include "dct.h"
#include "test_data.h"

CuSuite* CuGetSuite(void) 
{
	CuSuite* suite = CuSuiteNew();

	return suite;
}
