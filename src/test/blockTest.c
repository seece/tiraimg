
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "eks_math.h"
#include "CuTest.h"
#include "block.h"
#include "test_data.h"


void TestByteBlockInit(CuTest* tc) 
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

void TestFloatBlockInit(CuTest* tc) 
{
	struct FloatBlock* blockp = new_floatblock();

	int size = TIMG_BLOCK_SIZE;

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertDblEquals(tc, 0.0, blockp->data[y][x], 0.0);
		}
	}

	del_floatblock(blockp);
}

void TestBlockBias(CuTest* tc) 
{
	int size = TIMG_BLOCK_SIZE;
	struct ByteBlock input;
	memcpy(&input, &test_input, sizeof(input));
	struct FloatBlock output;

	float bias_amount = -128.0f;
	bias_block(&input, bias_amount, &output);

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertTrue(tc, (float)input.data[y][x] > output.data[y][x]);
			CuAssertTrue(tc, output.data[y][x] - (float)input.data[y][x] == bias_amount);
		}
	}
}

CuSuite* CuGetBlockSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestByteBlockInit);
	SUITE_ADD_TEST(suite, TestFloatBlockInit);
	SUITE_ADD_TEST(suite, TestBlockBias);
	return suite;
}
