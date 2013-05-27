/**
 * Includes the definitions of the two main data containers,
 * ByteBlock and FloatBlock and functions to operate them.
 */

#ifndef BLOCK_H_
#define BLOCK_H_

#include <stdint.h>

#define TIMG_BLOCK_SIZE 8 

/**
 * @brief A two dimensional byte array used to represent pixel values of a
 * single color channel, or quantized DCT coefficients before entropy
 * encoding. Has a fixed size.
 */
struct ByteBlock 
{
	/** Two-dimensional array of 8-bit data. */
	uint8_t data[TIMG_BLOCK_SIZE][TIMG_BLOCK_SIZE];
};

/** @brief A fixed size two-dimensional float array used as an intermediary container
 * in DCT and IDCT calculations.
 */
struct FloatBlock 
{
	/** Floating point data. */
	float data[TIMG_BLOCK_SIZE][TIMG_BLOCK_SIZE];
};

void byteblock_init(struct ByteBlock* blockp);
struct ByteBlock* byteblock_new();
void byteblock_del(struct ByteBlock* blockp);
void byteblock_print(struct ByteBlock* blockp);

void floatblock_init(struct FloatBlock* blockp);
struct FloatBlock* floatblock_new();
void floatblock_del(struct FloatBlock* blockp);
void byteblock_add(struct ByteBlock const* input,
	       	float bias_value, 
		struct FloatBlock* output);
void floatblock_print(struct FloatBlock* blockp);
void floatblock_multiply(struct FloatBlock* multiplier,
		struct FloatBlock* target);
void floatblock_add(
		struct FloatBlock const* input,
	       	float bias_value, 
		struct FloatBlock* output);

void byteblock_to_float(const struct ByteBlock *input, 
		struct FloatBlock* output);
void floatblock_to_byte(const struct FloatBlock *input, 
		struct ByteBlock* output);

//void floatblock_to_ycbcr(struct FloatBlock* blockp);

#endif
