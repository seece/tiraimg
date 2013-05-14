
#include <stdint.h>
#include "CuTest.h"
#include "block.h"
#include "dct.h"
#include "test_data.h"

void TestBlockInit(CuTest* tc) 
{
	struct ByteBlock* blockp = new_byteblock();

	int size = TIMG_BLOCK_SIZE;

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertIntEquals(tc, 0, blockp->data[y][x]);
		}
	}

	del_byteblock(blockp);
}

CuSuite* CuGetSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestBlockInit);
	return suite;
}
