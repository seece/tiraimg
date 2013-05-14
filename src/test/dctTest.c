
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "CuTest.h"
#include "block.h"
#include "dct.h"
#include "test_data.h"

static void byteblock_compare_test(CuTest* tc, struct ByteBlock* a, struct ByteBlock* b) 
{
	int size = TIMG_BLOCK_SIZE;

	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertIntEquals(tc, a->data[y][x], b->data[y][x]);
		}
	}
}

static bool double_compare(double a, double b) {
	const double thresold = 0.01;
	return fabs(a-b) <= thresold;
}

void TestDCT(CuTest* tc) 
{
	// This is a listing from https://en.wikipedia.org/wiki/Jpeg#Discrete_cosine_transform
	float reference[8][8] = {
		-415.38,  -30.19,  -61.20,   27.24,   56.12,  -20.10,   -2.39,    0.46, 
		4.47,  -21.86,  -60.76,   10.25,   13.15,   -7.09,   -8.54,    4.88, 
		-46.83,    7.37,   77.13,  -24.56,  -28.91,    9.93,    5.42,   -5.65, 
		-48.53,   12.07,   34.10,  -14.76,  -10.24,    6.30,    1.83,    1.95, 
		12.12,   -6.55,  -13.20,   -3.95,   -1.88,    1.75,   -2.79,    3.14, 
		-7.73,    2.91,    2.38,   -5.94,   -2.38,    0.94,    4.30,    1.85, 
		-1.03,    0.18,    0.42,   -2.42,   -0.88,   -3.02,    4.12,   -0.66, 
		-0.17,    0.14,   -1.07,   -4.19,   -1.17,   -0.10,    0.50,    1.68,
	};
	
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
			CuAssertDblEquals(tc, reference[y][x], output.data[y][x], 0.01);
		}
	}
}

void TestQuantization(CuTest* tc) 
{
	/* This is approximately theexpected result. The divisors
	 * are equivalent to the rounded entries of a scaled 
	 * quantization matrix at quality level 50.
	 */
	struct ByteBlock reference = {{
		52/3, 55/2, 61/2, 66/3,  70/5,  61/8,  64/10, 73/12,
		63/2, 59/2, 55/3, 90/4,  109/5, 85/12,  69/12, 72/11,
		62/3, 59/3, 68/3, 113/5, 144/8, 104/11, 66/14, 73/11,
		63/3, 58/3, 71/4, 122/6, 154/10, 106/17, 70/16, 69/12,
		67/4, 61/4, 68/7, 104/11, 126/14, 88/22,  68/21, 70/15,
		78/5, 65/7, 60/11, 70/13,  77/16,  68/12,  58/23, 75/18,
		85/10, 71/13, 64/16, 59/17,  55/21,  61/24,  65/24, 83/21,
		87/14, 79/18, 69/19, 68/20,  65/22,  76/20,  78/20, 94/20
	}};	

	/*
	struct FloatBlock reference = {{
		-415.38f, -30.19f, -61.20f, 27.24f, 56.13f, -20.10f, -2.39f, 0.46,
		4.47f, -21.86f, -60.76f, 10.25f, 13.15f, -7.09f, -8.54f, 4.88,
		-46.83f, 7.37f, 77.13f, -24.56f, -28.91f, 9.93f, 5.42f, -5.65,
		-48.53f, 12.07f, 34.10f, -14.76f, -10.24f, 6.30f, 1.83f, 1.95,
		12.12f, -6.55f, -13.20f, -3.95f, -1.88f, 1.75f, -2.79f, 3.14,
		-7.73f, 2.91f, 2.38f, -5.94f, -2.38f, 0.94f, 4.30f, 1.85,
		-1.03f, 0.18f, 0.42f, -2.42f, -0.88f, -3.02f, 4.12f, -0.66,
		-1.17f, 0.14f, -1.07f, -4.19f, -1.17f, -0.10f, 0.50f, 1.68
	}};
	*/

	int size = TIMG_BLOCK_SIZE;
	struct ByteBlock output;

	quantize_byteblock(&test_input, 90, &output);
	printf("output: \n");
	byteblock_print(&output);
	printf("\n");
	byteblock_print(&reference);

	// We allow some rounding errors.
	for (int y=0;y<size;y++) {
		for (int x=0;x<size;x++) {
			CuAssertTrue(tc, abs((int32_t)(reference.data[y][x] - output.data[y][x])) < 6);
		}
	}
}

void TestBlockBias(CuTest* tc) 
{
	int size = TIMG_BLOCK_SIZE;
	struct ByteBlock input;
	memcpy(&input, test_input, sizeof(input));
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

CuSuite* CuGetDCTSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestBlockBias);
	SUITE_ADD_TEST(suite, TestDCT);
	SUITE_ADD_TEST(suite, TestQuantization);
	return suite;
}
