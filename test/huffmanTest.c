#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CuTest.h"
#include "image/image.h"
#include "test_data.h"
#include "testHelpers.h"
#include "bitbuf.h"

// from http://stackoverflow.com/a/3974138
void printBits(size_t const size, void const * const ptr)
{
	unsigned char *b = (unsigned char*) ptr;
	unsigned char byte;
	int i, j;

	for (i=size-1;i>=0;i--)
	{
		for (j=7;j>=0;j--)
		{
			byte = b[i] & (1<<j);
			byte >>= j;
			printf("%u", byte);
		}
	}
	//puts("");
}

void TestBitBufferWrite(CuTest* tc)
{
	struct BitBuffer* buf = bitbuf_new(16);

	for (int32_t i=0;i<10;i++) {
		bitbuf_write_bit(buf, 1);
	}

	CuAssertIntEquals(tc, 1, buf->pos);
	CuAssertIntEquals(tc, 2, buf->bit_pos);
	CuAssertIntEquals(tc, 0xFF, buf->data[0]);
	CuAssertIntEquals(tc, 0xC0, buf->data[1]);

	bitbuf_del(buf);
}

void TestBitBufferWriteRandom(CuTest* tc)
{
	int32_t noise_length = 100;
	uint8_t bits[noise_length];
	
	struct BitBuffer* buf = bitbuf_new(8);
	srand(15);

	for (int32_t i=0;i<noise_length;i++) {
		uint8_t bit = rand() % 2;

		bits[i] = bit;
		bitbuf_write_bit(buf, bit);
		/*
		printf("%02x %02x\t", buf->data[0], buf->data[1]);
		printBits(1, &buf->data[0]);
		printf(" ");
		printBits(1, &buf->data[1]);
		printf(" \n");
		*/
	}

	for (int32_t i=0;i<noise_length;i++) {
		CuAssertIntEquals(tc, bits[i], 0 < (buf->data[i/8] & (1 << (7 - i % 8))));
	}

	bitbuf_del(buf);
}

CuSuite* CuGetHuffmanSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestBitBufferWrite);
	SUITE_ADD_TEST(suite, TestBitBufferWriteRandom);

	return suite;
}
