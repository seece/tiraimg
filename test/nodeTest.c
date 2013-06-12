
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

void TestSimpleTree(CuTest* tc)
{
	int32_t node_amount;
	struct Node* codes[256];
	node_amount = huffman_populate_forest(tree_data, sizeof(tree_data), codes);

	struct Node* tree = huffman_create_tree(codes, node_amount);

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

	node_del(tree);
}

void TestJoinNodes(CuTest* tc)
{
	struct Node* a = node_new();	
	struct Node* b = node_new();	

	a->weight= 1;
	b->weight= 2;

	struct Node* c = node_join(a, b);
	CuAssertIntEquals(tc, 3, c->weight);
	CuAssertIntEquals(tc, NODE_VALUE_NONE, c->value);

	node_del(a);
}

void TestLeafDetect(CuTest* tc)
{
	struct Node* a = node_new();	
	struct Node* b = node_new();	
	struct Node* c = node_join(a, b);

	CuAssertTrue(tc, node_is_leaf(a));
	CuAssertTrue(tc, node_is_leaf(b));
	CuAssertTrue(tc, !node_is_leaf(c));

	node_del(a);
}

void TestNodeCount(CuTest* tc)
{
	struct Node* a = node_new();	
	struct Node* b = node_new();	
	struct Node* c = node_join(a, b);

	CuAssertIntEquals(tc, 1, node_count_nodes(a));
	CuAssertIntEquals(tc, 1, node_count_nodes(b));
	CuAssertIntEquals(tc, 3, node_count_nodes(c));

	node_del(a);
}

void TestLeafCount(CuTest* tc)
{
	int32_t node_amount;

	struct Node* nodes[256];
	node_amount = huffman_populate_forest(tree_data, sizeof(tree_data), nodes);
	struct Node* tree = huffman_create_tree(nodes, node_amount);

	int32_t leaf_amount = 0;
	struct Node** leaves = get_leaf_nodes(tree, NULL, &leaf_amount);

	CuAssertIntEquals(tc, 4, leaf_amount);
	CuAssertPtrNotNull(tc, leaves);

	node_del(tree);
}


void TestTreeSerialization(CuTest* tc)
{
	struct Node* nodes[256];
	int32_t node_amount = huffman_populate_forest(tree_data, sizeof(tree_data), nodes);
	struct Node* tree = huffman_create_tree(nodes, node_amount);
	
	int32_t data_len = -1;
	uint8_t* treedata = node_serialize_tree(tree, &data_len);

	int32_t result_tree_len = -1;
	struct Node* result = node_unserialize_tree(treedata, data_len, &result_tree_len);

	free(treedata);
	node_del(tree);
}

CuSuite* CuGetNodeSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestSimpleTree);
	SUITE_ADD_TEST(suite, TestLeafCount);
	SUITE_ADD_TEST(suite, TestNodeCount);
	SUITE_ADD_TEST(suite, TestTreeSerialization);
	SUITE_ADD_TEST(suite, TestJoinNodes);
	SUITE_ADD_TEST(suite, TestLeafDetect);

	return suite;
}
