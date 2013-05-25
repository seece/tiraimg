
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "eks_math.h"
#include "CuTest.h"
#include "block.h"
#include "dct.h"
#include "jpeg.h"
#include "image/image.h"
#include "test_data.h"
#include "testHelpers.h"

void TestDCT(CuTest* tc) 
{
	int size = TIMG_BLOCK_SIZE;

	struct ByteBlock input;
	memcpy(&input, &test_input, sizeof(input));

	byteblock_print(&input);

	struct FloatBlock output;
	floatblock_init(&output);

	dct_calculate(&input, &output);
	floatblock_print(&output);

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertDblEquals(tc, test_dct_data.data[y][x], output.data[y][x], 0.01);
		}
	}
}

void TestIDCT(CuTest* tc) 
{
	int size = TIMG_BLOCK_SIZE;

	struct FloatBlock input;
	struct ByteBlock output;

	memcpy(&input, &test_dct_data, sizeof(input));
	byteblock_init(&output);

	dct_calculate_inverse(&input, &output);

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertIntEquals(tc, test_input.data[y][x], output.data[y][x]);
		}
	}
}

void TestTwoWayDCT(CuTest* tc) 
{
	int size = TIMG_BLOCK_SIZE;

	struct ByteBlock input;
	struct ByteBlock result;
	struct FloatBlock dct_coeffs;

	memcpy(&input, &test_input, sizeof(input));
	dct_calculate(&input, &dct_coeffs);

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertDblEquals(tc, test_dct_data.data[y][x], dct_coeffs.data[y][x], 0.01);
		}
	}

	dct_calculate_inverse(&dct_coeffs, &result);

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertIntEquals(tc, input.data[y][x], result.data[y][x]);
		}
	}
	
}

void TestQuantization(CuTest* tc) 
{
	/* This is approximately the expected result. The divisors
	 * are equivalent to the rounded entries of a scaled 
	 * quantization matrix at quality level 50.
	 */
	struct ByteBlock reference = {{
		{52/3, 55/2, 61/2, 66/3,  70/5,  61/8,  64/10, 73/12},
		{63/2, 59/2, 55/3, 90/4,  109/5, 85/12,  69/12, 72/11},
		{62/3, 59/3, 68/3, 113/5, 144/8, 104/11, 66/14, 73/11},
		{63/3, 58/3, 71/4, 122/6, 154/10, 106/17, 70/16, 69/12},
		{67/4, 61/4, 68/7, 104/11, 126/14, 88/22,  68/21, 70/15},
		{78/5, 65/7, 60/11, 70/13,  77/16,  68/12,  58/23, 75/18},
		{85/10, 71/13, 64/16, 59/17,  55/21,  61/24,  65/24, 83/21},
		{87/14, 79/18, 69/19, 68/20,  65/22,  76/20,  78/20, 94/20}
	}};	

	int size = TIMG_BLOCK_SIZE;
	struct ByteBlock output;

	dct_quantize_byteblock(&test_input, 90, &output);
	printf("output: \n");
	byteblock_print(&output);
	printf("\n");
	byteblock_print(&reference);

	// We allow some rounding errors to happen.
	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertTrue(tc, abs((int32_t)(reference.data[y][x] - output.data[y][x])) < 6);
		}
	}
}



CuSuite* CuGetDCTSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestDCT);
	SUITE_ADD_TEST(suite, TestIDCT);
	SUITE_ADD_TEST(suite, TestTwoWayDCT);
	SUITE_ADD_TEST(suite, TestQuantization);

	return suite;
}
