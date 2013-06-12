#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include "CuTest.h"
#include "eks_math.h"
#include "image/image.h"
#include "test_data.h"
#include "testHelpers.h"
#include "util/bitbuf.h"
#include "trie.h"
#include "huffman.h"

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
}

void TestBitBufferWrite(CuTest* tc)
{
	struct BitBuffer* buf = bitbuf_new(16);

	for (int32_t i=0;i<10;i++) {
		bitbuf_put_bit(buf, 1);
	}

	CuAssertIntEquals(tc, 1, buf->pos);
	CuAssertIntEquals(tc, 2, buf->bit_pos);
	CuAssertIntEquals(tc, 0xFF, buf->data[0]);
	CuAssertIntEquals(tc, 0xC0, buf->data[1]);

	bitbuf_del(buf);
}

void TestBitBufferWriteMultiple(CuTest* tc)
{
	struct BitBuffer* buf = bitbuf_new(16);

	bitbuf_put_bits(buf, 0x55, 7); // 1010101
	CuAssertIntEquals(tc, 7, buf->bit_pos);
	CuAssertIntEquals(tc, 0, buf->pos);
	CuAssertIntEquals(tc, 0xAA, buf->data[0]); // 0xAA = 10101010
	CuAssertIntEquals(tc, 0x00, buf->data[1]);

	bitbuf_put_bits(buf, 0x07, 3); // 111

	CuAssertIntEquals(tc, 2, buf->bit_pos);
	CuAssertIntEquals(tc, 1, buf->pos);
	CuAssertIntEquals(tc, 0xAB, buf->data[0]); // 0xAB = 10101011
	CuAssertIntEquals(tc, 0xC0, buf->data[1]); // 0xC0 = 11000000

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
		bitbuf_put_bit(buf, bit);
	}

	for (int32_t i=0;i<noise_length;i++) {
		CuAssertIntEquals(tc, bits[i], 0 < (buf->data[i/8] & (1 << (7 - i % 8))));
	}

	bitbuf_del(buf);
}

void TestBitBufferRead(CuTest* tc)
{
	struct BitBuffer* buf = bitbuf_new(16);

	for (int32_t i=0;i<100;i++) {
		uint8_t bit = 0;
	
		if (i == 1 || i == 45) {
			bit = 1;
		}

		bitbuf_put_bit(buf, bit);
	}

	CuAssertIntEquals(tc, 0, bitbuf_read_bit(buf, 0));
	CuAssertIntEquals(tc, 1, bitbuf_read_bit(buf, 1));
	CuAssertIntEquals(tc, 0, bitbuf_read_bit(buf, 16));
	CuAssertIntEquals(tc, 1, bitbuf_read_bit(buf, 45));

	bitbuf_del(buf);
}

void TestSimpleDistribution(CuTest* tc)
{
	int32_t node_amount;
	uint8_t data[] = {1, 1, 1, 5, 0, 52, 52};
	struct Node* code_trees[256];
	node_amount = huffman_populate_forest(data, sizeof(data), code_trees);

	CuAssertIntEquals(tc, 4, node_amount);
	CuAssertIntEquals(tc, 1, code_trees[0]->weight);
	CuAssertIntEquals(tc, 3, code_trees[1]->weight);
	CuAssertIntEquals(tc, 1, code_trees[5]->weight);
	CuAssertIntEquals(tc, 2, code_trees[52]->weight);

	//printf("node_amount: %d\n", node_amount);

	for (int32_t i=0;i<256;i++) {
		if (!code_trees[i])
			continue;

		node_del(code_trees[i]);

		//printf(" %d: %d\n", i, code_trees[i]->weight);
	}
}

void TestSymbolDistribution(CuTest* tc)
{
	int32_t code_amount = -1;

	struct Node* nodes[256];
	int32_t node_amount = huffman_populate_forest(tree_data, sizeof(tree_data), nodes);
	struct Node* tree = huffman_create_tree(nodes, node_amount);
	struct SymbolCode* codes = huffman_get_symbol_codes(tree, &code_amount);

	CuAssertIntEquals(tc, 0, codes[0].code);
	CuAssertIntEquals(tc, 1, codes[0].length);

	CuAssertIntEquals(tc, 4, codes[1].code);
	CuAssertIntEquals(tc, 3, codes[1].length);

	CuAssertIntEquals(tc, 5, codes[2].code);
	CuAssertIntEquals(tc, 3, codes[2].length);

	CuAssertIntEquals(tc, 3, codes[3].code);
	CuAssertIntEquals(tc, 2, codes[3].length);

	free(codes);
	node_del(tree);
}

void TestHuffmanCoding(CuTest* tc)
{
	uint8_t data[] = {0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x0A, 0x0B};
	int32_t data_len = sizeof(data);
	int32_t code_amount = -1;

	struct Node* nodes[256];
	int32_t node_amount = huffman_populate_forest(data, sizeof(data), nodes);
	struct Node* tree = huffman_create_tree(nodes, node_amount);
	struct SymbolCode* codes = huffman_get_symbol_codes(tree, &code_amount);

	struct SymbolCode* code_table[256] = {NULL};
	struct BitBuffer* buf = bitbuf_new(16);

	// propagate the code table
	for (int32_t i=0;i<code_amount;i++) {
		assert(codes[i].value < 256);
		assert(codes[i].value >= 0);
		code_table[codes[i].value] = &codes[i];
	}

	for (int32_t i=0;i<data_len;i++)  {
		uint8_t value = data[i];
		struct SymbolCode* code = code_table[value];

		if (!code) {
			printf("0x%X: code not found from code table!\n", value);
			continue;
		}

		bitbuf_put_bits(buf, code->code, code->length); 
	}

	bitbuf_del(buf);
	free(codes);
	node_del(tree);
}

void TestHuffmanInterface(CuTest* tc)
{
	uint8_t data[] = {0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x0A, 0x0B};
	int32_t data_len = sizeof(data);
	uint64_t huff_len= 0;
	uint8_t* huff_data = huffman_encode(data, (uint64_t)data_len, &huff_len);

	uint64_t result_len = 0;
	uint8_t* result_data = huffman_decode(huff_data, huff_len, &result_len);
	CuAssertIntEquals(tc, data_len, result_len);
	check_arrays_equal(tc, result_data, data, result_len);
}

CuSuite* CuGetHuffmanSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestBitBufferWrite);
	SUITE_ADD_TEST(suite, TestBitBufferWriteMultiple);
	SUITE_ADD_TEST(suite, TestBitBufferWriteRandom);
	SUITE_ADD_TEST(suite, TestBitBufferRead);
	SUITE_ADD_TEST(suite, TestSimpleDistribution);
	SUITE_ADD_TEST(suite, TestSymbolDistribution);
	SUITE_ADD_TEST(suite, TestHuffmanCoding);
	SUITE_ADD_TEST(suite, TestHuffmanInterface);

	return suite;
}
