/**
 * @file compress.h
 * @brief High-level image compression functions.
 */


#ifndef COMPRESS_H_
#define COMPRESS_H_

#include "block.h"
#include "image/image.h"

#define COMPRESS_KEEP_YCBCR (1 << 0)

void compress_blockarray_dct(struct BlockArray* arrayp, int32_t quality);
void compress_blockarray_dct_inverse(struct BlockArray* arrayp, int32_t quality);

int32_t compress_block_encode(const struct ByteBlock* block, uint8_t* output);
uint8_t* compress_image_full(const struct Image* imagep, int32_t quality,
	uint64_t* length);
struct Image* decompress_image_full(uint8_t* data, uint64_t length, uint32_t flags);

#endif
