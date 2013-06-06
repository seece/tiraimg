#ifndef HUFFMAN_H_
#define HUFFMAN_H_
#include <stdint.h>

uint8_t* huffman_encode(uint8_t* input, uint64_t length, uint64_t* length_result);
uint8_t* huffman_decode(uint8_t* input, uint64_t length, uint64_t* length_result);

int32_t huffman_populate_forest(const uint8_t* data, uint64_t data_len, struct Node* codes[]);
struct Node* huffman_create_tree(struct Node* codes[], int32_t amount);
#endif
