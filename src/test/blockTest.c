
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include "eks_math.h"
#include "CuTest.h"
#include "block.h"
#include "test_data.h"

static const int size = TIMG_BLOCK_SIZE;

void generate_random_floatblock(struct FloatBlock* blockp, float low, float high)
{
	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			blockp->data[y][x] = low + (float)rand()/((float)RAND_MAX/(high-low));
		}
	}
}

void TestByteBlockInit(CuTest* tc) 
{
	struct ByteBlock* blockp = new_byteblock();

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

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertDblEquals(tc, 0.0, blockp->data[y][x], 0.0);
		}
	}

	del_floatblock(blockp);
}

void TestBlockBias(CuTest* tc) 
{

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

void TestBlockMultiply(CuTest* tc) 
{
	struct FloatBlock first;
	struct FloatBlock second;
	struct FloatBlock third;

	srand(13);
	generate_random_floatblock(&first, 0.0, 10.0);
	srand(14);
	generate_random_floatblock(&second, -10.0, 0.0);
	srand(14);
	generate_random_floatblock(&third, -10.0, 0.0);

	multiply_floatblock(&first, &second);

	// The third block contains the original values of the second block.

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertDblEquals(tc, first.data[y][x] * third.data[y][x], 
					second.data[y][x], 0.0001);
		}
	}
}

CuSuite* CuGetBlockSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestByteBlockInit);
	SUITE_ADD_TEST(suite, TestFloatBlockInit);
	SUITE_ADD_TEST(suite, TestBlockBias);
	SUITE_ADD_TEST(suite, TestBlockMultiply);
	return suite;
}
