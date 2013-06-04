#ifndef HUFFMAN_H_
#define HUFFMAN_H_
#include <stdint.h>

uint8_t* huffman_encode(uint8_t* input, uint64_t length, uint64_t* length_result);
uint8_t* huffman_decode(uint8_t* input, uint64_t length, uint64_t* length_result);

#endif
