#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "bitbuf.h"

uint8_t* huffman_encode(uint8_t* input, uint64_t length, uint64_t* length_result) 
{
	assert(input);
	assert(length > 0);
	assert(length_result);

	struct BitBuffer* tempbuf = bitbuf_new(64);

	uint32_t buffersize = length + 4;

	uint8_t* newbuffer = malloc(length);
	memcpy(newbuffer, input, length);

	*length_result = length;

	return newbuffer;
}

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
