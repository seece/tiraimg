
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "eks_math.h"
#include "CuTest.h"
#include "block.h"
#include "test_data.h"

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
	SUITE_ADD_TEST(suite, TestBlockBias);
	return suite;
}
