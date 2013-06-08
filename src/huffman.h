#ifndef HUFFMAN_H_
#define HUFFMAN_H_
#include <stdint.h>
#include "trie.h"

uint8_t* huffman_encode(uint8_t* input, uint64_t length, uint64_t* length_result);
uint8_t* huffman_decode(uint8_t* input, uint64_t length, uint64_t* length_result);

int32_t huffman_populate_forest(const uint8_t* data, uint64_t data_len, struct Node* codes[]);
struct Node* huffman_create_tree(struct Node* codes[], int32_t amount);
struct Node** get_leaf_nodes(struct Node* node, struct Node** leaves_in, 
		int32_t* amount_out);
struct SymbolCode* huffman_get_symbol_codes(struct Node* tree, int32_t* amount_out);
#endif
