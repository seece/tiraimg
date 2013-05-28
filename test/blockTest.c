
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
#include "testHelpers.h"
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
	struct ByteBlock* blockp = byteblock_new();

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertIntEquals(tc, 0, blockp->data[y][x]);
		}
	}

	byteblock_del(blockp);
}

void TestFloatBlockInit(CuTest* tc) 
{
	struct FloatBlock* blockp = floatblock_new();

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertDblEquals(tc, 0.0, blockp->data[y][x], 0.0);
		}
	}

	floatblock_del(blockp);
}

void TestBlockBias(CuTest* tc) 
{

	struct ByteBlock input;
	memcpy(&input, &test_input, sizeof(input));
	struct FloatBlock output;

	float bias_amount = -128.0f;
	byteblock_add(&input, bias_amount, &output);

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

	floatblock_multiply(&first, &second);

	// The third block contains the original values of the second block.

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertDblEquals(tc, first.data[y][x] * third.data[y][x], 
					second.data[y][x], 0.0001);
		}
	}
}

void TestByteBlockToFloatConversion(CuTest* tc) 
{
	struct FloatBlock output;	
	
	void byteblock_to_float(const struct ByteBlock *input, 
			struct FloatBlock* output);

	byteblock_to_float(&test_input, &output);

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertDblEquals(tc, (float)test_input.data[y][x], 
					output.data[y][x], 0.0001);
		}
	}
}

void TestByteBlockPack(CuTest* tc) 
{
	struct ByteBlock input;
	struct ByteBlock output;
	struct ByteBlock result;

	for (int i=0;i<64;i++) {
		input.data[i/8][i%8] = i;
		output.data[i/8][i%8] = 0;
	}

	printf("blockpack input:\n");

	byteblock_print(&input);

	byteblock_pack(&input, &output);
	printf("blockpack output\n");
	byteblock_print(&output);
	printf("\n");

	byteblock_compare_test(tc, &zigzag_order, &output);

	byteblock_unpack(&output, &result);
	byteblock_compare_test(tc, &input, &result);
}

CuSuite* CuGetBlockSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestByteBlockInit);
	SUITE_ADD_TEST(suite, TestFloatBlockInit);
	SUITE_ADD_TEST(suite, TestBlockBias);
	SUITE_ADD_TEST(suite, TestBlockMultiply);
	SUITE_ADD_TEST(suite, TestByteBlockToFloatConversion);
	SUITE_ADD_TEST(suite, TestByteBlockPack);
	//SUITE_ADD_TEST(suite, TestFloatColorspaceConversion);
	return suite;
}
