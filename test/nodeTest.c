
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include "eks_math.h"
#include "CuTest.h"
#include "testHelpers.h"
#include "test_data.h"
#include "util/bitbuf.h"
#include "trie.h"
#include "huffman.h"

// source: http://stackoverflow.com/a/29865
void hexdump(void *ptr, int buflen) {
	unsigned char *buf = (unsigned char*)ptr;
	int i, j;
	for (i=0; i<buflen; i+=16) {
		printf("%06x: ", i);
		for (j=0; j<16; j++) 
			if (i+j < buflen)
				printf("%02x ", buf[i+j]);
			else
				printf("   ");
		printf(" ");
		for (j=0; j<16; j++) 
			if (i+j < buflen)
				printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
		printf("\n");
	}
}



static void print_tree(struct Node* root, int32_t level)
{
	//printf("depth: %d\n", depth);

	if (root == NULL)
		return;

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
	struct Node* codes[256];
	node_amount = huffman_populate_forest(tree_data, sizeof(tree_data), codes);

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

	node_del(tree);
}

void TestLeafCount(CuTest* tc)
{
	int32_t node_amount;

	struct Node* nodes[256];
	node_amount = huffman_populate_forest(tree_data, sizeof(tree_data), nodes);
	struct Node* tree = huffman_create_tree(nodes, node_amount);

	printf("das tree\n");
	print_tree(tree, 0);

	int32_t leaf_amount = 0;
	struct Node** leaves = get_leaf_nodes(tree, NULL, &leaf_amount);
	int32_t count_node_amount = node_count_nodes(tree);
	//printf("NODES: %d\n", count_node_amount);

	CuAssertIntEquals(tc, 4, leaf_amount);
	CuAssertPtrNotNull(tc, leaves);

	node_del(tree);
}


void TestTreeSerialization(CuTest* tc)
{
	int32_t code_amount = -1;

	struct Node* nodes[256];
	int32_t node_amount = huffman_populate_forest(tree_data, sizeof(tree_data), nodes);
	struct Node* tree = huffman_create_tree(nodes, node_amount);
	
	int32_t data_len = -1;
	// uint8_t* node_serialize_tree(struct Node* tree, int32_t* length_out)
	uint8_t* treedata = node_serialize_tree(tree, &data_len);

	printf("data len: %d\n", data_len);
	hexdump(treedata, 4);
	printf("\n");
	hexdump(treedata+4, data_len-4);
	printf("\n");

	struct Node* result = node_unserialize_tree(treedata, data_len);
	printf("unserialized:\n");
	print_tree(result, 0);

	free(treedata);
	node_del(tree);
}

CuSuite* CuGetNodeSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestSimpleTree);
	SUITE_ADD_TEST(suite, TestLeafCount);
	SUITE_ADD_TEST(suite, TestTreeSerialization);

	return suite;
}
