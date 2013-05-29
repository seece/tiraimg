/**
 * @file compress.h
 * @brief High-level image compression functions.
 */


#ifndef COMPRESS_H_
#define COMPRESS_H_

#include "block.h"
#include "image/image.h"

void compress_blockarray_dct(struct BlockArray* arrayp, int32_t quality);
void compress_blockarray_dct_inverse(struct BlockArray* arrayp, int32_t quality);
int32_t compress_block_encode(const struct ByteBlock* block, uint8_t* output);
#endif
