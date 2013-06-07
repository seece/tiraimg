#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "bitbuf.h"
#include "trie.h"

#define HUFFMAN_MAX_CODES 256
static struct Node* code_trees[HUFFMAN_MAX_CODES];
static int32_t node_amount = 0;

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
	//struct Node* smallest[2] = {NULL, NULL};
	//int32_t smallest_ind[2] = {-1, -1};
	int32_t count = 0;

	// copy all the created nodes to the tree array
	for (int32_t i=0;i<HUFFMAN_MAX_CODES;i++) {
		if (!codes[i])	
			continue;

		trees[count] = codes[i];

		count++;
	}

	assert(count == amount);
	printf("distinct code count: %d\n", count);

	// build the whole tree
	int32_t a, b;

	while (count > 1) {
		a = pick_smallest(trees, amount);

		struct Node* ap = trees[a];
		trees[a] = NULL; // we don't want to pick the same pointer twice

		b = pick_smallest(trees, amount);

		//printf("ind: %d, p: %p\n", b, trees[b]);
		printf("joining %d & %d, count: %d\n", a, b, count);

		trees[a] = node_join(ap, trees[b]);
		trees[b] = NULL;

		count--;
	}

	assert(count == 1);

	int32_t final_ind = pick_smallest(trees, amount);
	return trees[final_ind];

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

	struct BitBuffer* tempbuf = bitbuf_new(64);

	uint32_t buffersize = length + 4;

	// first calculate the byte distribution
	// and then join the generated trees to get the
	// final huffman tree
	node_amount = huffman_populate_forest(input, length, code_trees);
	struct Node* tree = huffman_create_tree(code_trees, node_amount);
	
	uint8_t* newbuffer = malloc(length);
	memcpy(newbuffer, input, length);

	*length_result = length;

	bitbuf_del(tempbuf);
	node_del(tree);

	return newbuffer;
}

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

	uint8_t* newbuffer = malloc(length);
	memcpy(newbuffer, input, length);

	*length_result = length;

	return newbuffer;
}
