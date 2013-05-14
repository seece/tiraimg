
#include <stdint.h>
#include "CuTest.h"
#include "block.h"
#include "dct.h"


void TestTrivial(CuTest* tc) 
{
	CuAssertTrue(tc, 1==1);
}

void TestBlockInit(CuTest* tc) 
{
	struct ByteBlock* blockp = new_byteblock();

	int size = TIMG_BLOCK_SIZE;

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertIntEquals(tc, 0, blockp->data[x][y]);
		}
	}

	del_byteblock(blockp);
}

void TestDCT(CuTest* tc) 
{
	int size = TIMG_BLOCK_SIZE;

	struct ByteBlock input = {{
		{52, 55, 61, 66,  70,  61,  64, 73 },
		{63, 59, 55, 90,  109, 85,  69, 72 },
		{62, 59, 68, 113, 144, 104, 66, 73 },
		{63, 58, 71, 122, 154, 106, 70, 69 },
		{67, 61, 68, 104, 126, 88,  68, 70 },
		{79, 65, 60, 70,  77,  68,  58, 75 },
		{85, 71, 64, 59,  55,  61,  65, 83 },
		{87, 79, 69, 68,  65,  76,  78, 94 }
	}};

	byteblock_print(&input);

	struct FloatBlock output;
	floatblock_init(&output);

	floatblock_print(&output);
	dct_calculate(&input, &output);


	//del_block(blockp);
}

CuSuite* CuGetSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite,TestTrivial);
	SUITE_ADD_TEST(suite,TestBlockInit);
	SUITE_ADD_TEST(suite, TestDCT);
	return suite;
}
