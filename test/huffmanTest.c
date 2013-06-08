#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CuTest.h"
#include "eks_math.h"
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

static int32_t tree_depth(struct Node* root, int32_t depth)
{
	if (!root)
		return depth;

	return MAX(tree_depth(root->left, depth+1), tree_depth(root->right, depth+1));
}

static void print_tree(struct Node* root, int32_t level)
{
	//printf("depth: %d\n", depth);

	if (root == NULL)
		return;

	int32_t depth = tree_depth(root, 0);
	printf("%*s", level*4, " ");
	printf("  ");

	if (root->value == NODE_VALUE_NONE) {
		printf("(x) %d", root->weight);
	} else {
		printf("(%d) %d", root->value, root->weight);
	}

	//printf("(%3d)", root->value);
	printf("\n");

	//printf("%d: %p:\t%9p\t%9p weight: %d val:%d\n", level, root, root->left, root->right, root->weight, root->value);

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
	//printf("the returned tree, root: %p:\n", tree);
	//print_tree(tree, 0);

	struct SymbolCode sym;
	sym = node_get_code(tree, 5);
	CuAssertIntEquals(tc, 5, sym.code);
	CuAssertIntEquals(tc, 3, sym.length);
	sym = node_get_code(tree, 1);
	CuAssertIntEquals(tc, 0, sym.code);
	CuAssertIntEquals(tc, 1, sym.length);
	sym = node_get_code(tree, 52);
	CuAssertIntEquals(tc, 3, sym.code);
	CuAssertIntEquals(tc, 2, sym.length);

	/*
	printf("symbol %d: len: %d, code: ", 5, sym.length);
	printBits(4, &sym.code);
	printf("\n");
	*/
}

void TestSymbolDistribution(CuTest* tc)
{
	int32_t node_amount;
	uint8_t data[] = {1, 1, 1, 5, 0, 52, 52};
	struct Node* codes[256];
	node_amount = huffman_populate_forest(data, sizeof(data), codes);
	struct Node* tree = huffman_create_tree(codes, node_amount);

	printf("das tree\n");
	print_tree(tree, 0);

	int32_t leaf_amount = 0;
	struct Node** leaves = get_leaf_nodes(tree, NULL, &leaf_amount);

	printf("leaf amount: %d\n", leaf_amount);

	for (int32_t i=0;i<leaf_amount;i++) {
		printf("node: (%d), weight: %d %p\n", leaves[i]->value, leaves[i]->weight, leaves[i]);
	}

	node_del(tree);
}

CuSuite* CuGetHuffmanSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestBitBufferWrite);
	SUITE_ADD_TEST(suite, TestBitBufferWriteRandom);
	SUITE_ADD_TEST(suite, TestBitBufferRead);
	SUITE_ADD_TEST(suite, TestSimpleDistribution);
	SUITE_ADD_TEST(suite, TestSimpleTree);
	SUITE_ADD_TEST(suite, TestSymbolDistribution);

	return suite;
}
