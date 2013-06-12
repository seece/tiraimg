#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "tiraimg.h"
#include "eks_math.h"
#include "util/bitbuf.h"
#include "trie.h"

#define HUFFMAN_MAX_CODES 256

/**
 * @brief Calculates the distribution, and creates the nodes from a byte 
 * array.
 *
 * @param data source data 
 * @param data_len source data length
 * @param codes[] node pointer array for output
 *
 * @return number of distinct codes found
 */
int32_t huffman_populate_forest(const uint8_t* data, uint64_t data_len, struct Node* codes[])
{
	assert(data);
	assert(data_len > 0);
	assert(codes);

	int32_t amount = 0;

	for (int32_t i=0;i<HUFFMAN_MAX_CODES;i++) {
		codes[i] = NULL;
	}

	for (uint64_t i=0;i<data_len;i++) {
		uint8_t byte = data[i];

		if (codes[byte] == NULL) {
			codes[byte] = node_new();
			codes[byte]->weight = 0;
			codes[byte]->value = byte;
			amount++;
		}

		codes[byte]->weight++;
	}

	return amount;
}

/**
 * @brief Picks the node with the smallest weight from the 
 * given array. NULL pointers are simply skipped.
 *
 * @param trees[] node array, the haystack
 * @param amount amount of nodes in the array
 *
 * @return index of the smallest node
 */
static int32_t pick_smallest(struct Node* trees[], int32_t amount)
{
	int32_t smallest_id = -1;
	int32_t smallest_weight = 0;

	for (int32_t i=0;i<amount;i++) {
		if (!trees[i]) 
			continue;

		if (smallest_id == -1 || (trees[i]->weight < smallest_weight)) {
			smallest_id = i;
			smallest_weight = trees[i]->weight;
		}
	}

	return smallest_id;
}

/**
 * @brief Creates a huffman tree from the given code weights.
 *
 * @param codes[] the nodes with calculated weights
 * @param amount amount fo nodes in codes[] array
 *
 * @return the generated tree
 */
struct Node* huffman_create_tree(struct Node* codes[], int32_t amount)
{
	struct Node* trees[amount];
	int32_t count = 0;

	// copy all the created nodes to the tree array
	for (int32_t i=0;i<HUFFMAN_MAX_CODES;i++) {
		if (!codes[i])	
			continue;

		trees[count] = codes[i];

		count++;
	}

	assert(count == amount);

	// build the whole tree
	int32_t a, b;

	while (count > 1) {
		a = pick_smallest(trees, amount);

		struct Node* ap = trees[a];
		trees[a] = NULL; // we don't want to pick the same pointer twice

		b = pick_smallest(trees, amount);

		trees[a] = node_join(ap, trees[b]);
		trees[b] = NULL;

		count--;
	}

	assert(count == 1);

	int32_t final_ind = pick_smallest(trees, amount);
	return trees[final_ind];
}

/**
 * @brief A recursive function that finds all leaf nodes from the given tree.
 * The leaf node array is allocated dynamically if the leaves_in parameter is NULL.
 *
 * @param node the root node
 * @param leaves_in leaf array, on the initial call this should be NULL
 * @param amount_out number of leaves is saved here
 *
 * @return pointer to the allocated node array with amount_out members
 */
struct Node** get_leaf_nodes(struct Node* node, struct Node** leaves_in, 
		int32_t* amount_out)
{
	struct Node** leaves = leaves_in;

	if (leaves == NULL) {
		int32_t leaf_amount = node_count_leaf_nodes(node);
		leaves = calloc(leaf_amount, sizeof(struct Node*));
	}

	if (!node)
		return leaves;

	if (node_is_leaf(node)) {
		leaves[*amount_out] = node;
		(*amount_out)++;

		return leaves;
	} 

	get_leaf_nodes(node->left, leaves, amount_out);	
	get_leaf_nodes(node->right, leaves, amount_out);	

	return leaves;
}


/**
 * @brief Creates a SymbolCode list from the given Huffman-tree.
 *
 * @param tree the tree to use as a source
 * @param amount_out how many leaves (codes) were added to the list
 *
 * @return the allocated code list
 */
struct SymbolCode* huffman_get_symbol_codes(struct Node* tree, int32_t* amount_out)
{
	int32_t amount = 0;
	struct Node** leaves = get_leaf_nodes(tree, NULL, &amount);
	assert(amount > 0);
	struct SymbolCode* codes = calloc(amount, sizeof(struct SymbolCode));

	for (int32_t i=0;i<amount;i++)
	{
		codes[i] = node_get_code(tree, leaves[i]->value);
	}

	*amount_out = amount;

	return codes;
}

/**
 * @brief Encodes a given byte array with huffman coding. Currently just
 * a placeholder.
 *
 * @param input the source data
 * @param length source data length in bytes 
 * @param length_result output for the length of the resulting compressed buffer
 *
 * @return compressed data
 */
uint8_t* huffman_encode(uint8_t* input, uint64_t length, uint64_t* length_result) 
{
	assert(input);
	assert(length > 0);
	assert(length_result);
	assert(length < (1 << 31)); // we store the symbol amount as uint32_t

	int32_t code_amount = -1;

	struct Node* nodes[256];
	int32_t node_amount = huffman_populate_forest(input, length, nodes);
	struct Node* tree = huffman_create_tree(nodes, node_amount);
	struct SymbolCode* codes = huffman_get_symbol_codes(tree, &code_amount);
	
	struct SymbolCode* code_table[256] = {NULL};
	struct BitBuffer* buf = bitbuf_new(64);

	// propagate the code table
	for (int32_t i=0;i<code_amount;i++) {
		assert(codes[i].value < 256);
		assert(codes[i].value >= 0);
		code_table[codes[i].value] = &codes[i];
	}

	if (global_message_level >= TIMG_MSG_VERBOSE)
		printf("Huffman code count: %d\n", code_amount);

	for (int32_t i=0;i<length;i++)  {
		uint8_t value = input[i];
		struct SymbolCode* code = code_table[value];

		if (!code) {
			fprintf(stderr, "0x%X: code not found from code table!\n", value);
			continue;
		}

		bitbuf_put_bits(buf, code->code, code->length); 
	}

	int32_t tree_data_len = -1;
	uint8_t* tree_data = node_serialize_tree(tree, &tree_data_len);
	assert(tree_data);
	assert(tree_data_len > 0);
	uint32_t symbol_count = (uint32_t)length;

	int32_t headersize =  tree_data_len + 4;
	int32_t output_len = headersize + (buf->pos + 1);
	// huffman tree + amount of symbols (uint32_t) + symbol stream
	uint8_t* newbuffer = malloc(output_len);
	memcpy(&newbuffer[0], tree_data, tree_data_len); // huffman tree
	memcpy(&newbuffer[tree_data_len], &symbol_count, 4); // symbol amount
	memcpy(&newbuffer[headersize], buf->data, buf->pos + 1); // symbol stream

	*length_result = output_len;

	free(tree_data);
	free(codes);
	bitbuf_del(buf);
	node_del(tree);
	return newbuffer;
}


/**
 * @brief Reads a single bit from the given integer.
 *
 * @param a target integer
 * @param x bit position
 *
 * @return 0 or 1, depending on the bit's value
 */
#define READ_BIT(a, x) (((a) & (1 << (x))) != 0 ? 1 : 0)

/**
 * @brief Decompresses a huffman coded byte array.
 *
 * @param input source data 
 * @param length source data length
 * @param length_result output for the decompressed length
 *
 * @return decompressed data 
 */
uint8_t* huffman_decode(uint8_t* input, uint64_t length, uint64_t* length_result) 
{
	assert(input);
	assert(length > 0);
	assert(length_result);

	int32_t tree_len = -1;
	uint32_t symbol_count = 0;
	uint64_t pos = 0;
	// TODO fix that ugly uint64_t -> int32_t cast
	struct Node* tree = node_unserialize_tree(input, (int32_t) length, &tree_len);
	assert(tree);
	assert(tree_len > 0);

	if (global_message_level >= TIMG_MSG_HUFFMAN)
		node_print_tree(tree, 0);

	pos+=tree_len;

	memcpy(&symbol_count, &input[pos], 4);

	pos+=4;

	uint8_t* output_data = malloc(symbol_count);
	struct Node* node = tree;
	uint64_t out_pos=0;
	uint64_t cur_pos=0;
	int32_t cur_bit=0;

	while (out_pos < symbol_count) {
		if (node_is_leaf(node)) {
			output_data[out_pos] = node->value;
			out_pos++;
			node = tree;
			continue;
		}

		int32_t bit = READ_BIT(input[pos + cur_pos], 7-cur_bit);

		cur_bit++;

		if (cur_bit > 7) {
			cur_pos++;
			cur_bit = 0;
		}

		if (bit) {
			node = node->right;
		} else {
			node = node->left;
		}

	}

	*length_result = symbol_count;
	node_del(tree);

	return output_data;
}

#undef READ_BIT
