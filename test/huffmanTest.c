#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CuTest.h"
#include "image/image.h"
#include "test_data.h"
#include "testHelpers.h"
#include "bitbuf.h"
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
	//puts("");
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
	CuAssertIntEquals(tc, 1, code_trees[0]->value);
	CuAssertIntEquals(tc, 3, code_trees[1]->value);
	CuAssertIntEquals(tc, 1, code_trees[5]->value);
	CuAssertIntEquals(tc, 2, code_trees[52]->value);

	//printf("node_amount: %d\n", node_amount);

	for (int32_t i=0;i<256;i++) {
		if (!code_trees[i])
			continue;

		node_del(code_trees[i]);

		//printf(" %d: %d\n", i, code_trees[i]->value);
	}
}

static void print_tree(struct Node* root, int32_t level)
{
	if (root == NULL)
		return;

	printf("%d: %p:\t%p\t%p\t%d\n", level, root, root->left, root->right, root->value);

	print_tree(root->left, level+1);
	print_tree(root->right, level+1);
}

void TestSimpleTree(CuTest* tc)
{
	int32_t node_amount;
	uint8_t data[] = {1, 1, 1, 5, 0, 52, 52};
	struct Node* codes[256];
	node_amount = huffman_populate_forest(data, sizeof(data), codes);

	struct Node* tree = huffman_create_tree(codes, node_amount);
	printf("the returned tree, root: %p:\n", tree);
	print_tree(tree, 0);
}

CuSuite* CuGetHuffmanSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestBitBufferWrite);
	SUITE_ADD_TEST(suite, TestBitBufferWriteRandom);
	SUITE_ADD_TEST(suite, TestBitBufferRead);
	SUITE_ADD_TEST(suite, TestSimpleDistribution);
	SUITE_ADD_TEST(suite, TestSimpleTree);

	return suite;
}
