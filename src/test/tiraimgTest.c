
#include "CuTest.h"
#include "block.h"


void TestTrivial(CuTest* tc) 
{
	CuAssertTrue(tc, 1==1);
}

void TestBlockInit(CuTest* tc) 
{
	struct Block* blockp = new_block();

	int size = TIMG_BLOCK_SIZE;

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertIntEquals(tc, 0, blockp->data[x][y]);
		}
	}
	del_block(blockp);
}

CuSuite* CuGetSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestTrivial);
	SUITE_ADD_TEST(suite, TestBlockInit);
	return suite;
}
