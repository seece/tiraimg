
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "CuTest.h"
#include "block.h"
#include "dct.h"
#include "test_data.h"

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
	return suite;
}
