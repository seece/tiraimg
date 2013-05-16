#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include "block.h"


/**
 * @brief Initializes ByteBlock.
 *
 * @param blockp Pointer to the ByteBlock to initialize.
 */
void byteblock_init(struct ByteBlock* blockp)
{
	assert(blockp != NULL);
	int32_t size = TIMG_BLOCK_SIZE;

	for (int32_t i=0;i<size*size;i++) {
		blockp->data[i % size][i / size] = 0;
	}

}


/**
 * @brief Allocates memory for a new ByteBlock.
 *
 * @return Pointer to the new ByteBlock.
 */
struct ByteBlock* new_byteblock() 
{
	struct ByteBlock* blockp = malloc(sizeof(struct ByteBlock));
	byteblock_init(blockp);
	return blockp;
}


/**
 * @brief Frees memory of a ByteBlock.
 *
 * @param blockp Pointer to the ByteBlock.
 */
void del_byteblock(struct ByteBlock* blockp)
{
	assert(blockp != NULL);
	free(blockp);
}


/**
 * @brief Executes a callback function on every member of a FloatBlock
 * or ByteBlock.
 *
 * @param blockp Pointer to the block to iterate over.
 * @param func The callback function to call.
 */
static void block_map(
		void* blockp,
		void (*func)(void* valuep, int32_t x, int32_t y)) 
{
	int32_t size = TIMG_BLOCK_SIZE;
	struct ByteBlock* real_blockp = (struct ByteBlock*) blockp;

	for (int32_t i=0;i<size*size;i++) {
		int32_t x = i % size;
		int32_t y = i / size; 

		func((void *)(&real_blockp->data), x, y);
	}
}


/**
 * @brief Initializes a FloatBlock with value 0.0f.
 *
 * @param blockp Pointer to the FloatBlock to initialize.
 */
void floatblock_init(struct FloatBlock* blockp)
{
	assert(blockp != NULL);
	int32_t size = TIMG_BLOCK_SIZE;

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			blockp->data[y][x] = 0.0f;
		}
	}

}


/**
 * @brief Allocates memory for a new FloatBlock.
 *
 * @return Pointer to the new FloatBlock.
 */
struct FloatBlock* new_floatblock() 
{
	struct FloatBlock* blockp = malloc(sizeof(struct FloatBlock));
	floatblock_init(blockp);
	return blockp;
}


/**
 * @brief Frees memory of a floatblock.
 *
 * @param blockp Pointer to the FloatBlock to be freed.
 */
void del_floatblock(struct FloatBlock* blockp)
{
	assert(blockp != NULL);
	free(blockp);
}


/**
 * @brief Adds a certain floating point value to the each value of a FloatBlock.
 *
 * @param input The FloatBlock to read the original values from.
 * @param bias_value The value to add to all values of this FloatBlock.
 * @param output Result will be stored in this FloatBlock.
 */
void bias_block(
		struct ByteBlock const* input,
	       	float bias_value, 
		struct FloatBlock* output) 
{
	int size = TIMG_BLOCK_SIZE;

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			float val = (float)input->data[y][x];
			output->data[y][x] = val + bias_value;
		}
	}

}


/**
 * @brief Converts a ByteBlock to a FloatBlock.
 *
 * @param input The original ByteBlock.
 * @param output The new FloatBlock.
 */
void byteblock_to_float(struct ByteBlock *input, 
		struct FloatBlock* output)
{
	int size = TIMG_BLOCK_SIZE;

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			output->data[y][x] = (float)input->data[y][x];
		}
	}
}

/* Some debug helper functions. */


/**
 * @brief byteblock_print callback function.
 */
static void byte_print_callback(void* valuep, int32_t x, int32_t y) 
{
	printf("%3d ", ((struct ByteBlock*)valuep)->data[y][x]);

	if (x==TIMG_BLOCK_SIZE-1) {
		printf("\n");
	}
}

/**
 * @brief floatblock_print callback function.
 */
static void float_print_callback(void* valuep, int32_t x, int32_t y) 
{
	printf("%7.2f ", ((struct FloatBlock*)valuep)->data[y][x]);

	if (x==TIMG_BLOCK_SIZE-1) {
		printf("\n");
	}
}


/**
 * @brief Prints the values of a ByteBlock.
 *
 * @param blockp Pointer to the ByteBlock whose values should be printed.
 */
void byteblock_print(struct ByteBlock* blockp) 
{
	block_map((void *)blockp, byte_print_callback);
}

/**
 * @brief Prints the values of a FloatBlock.
 *
 * @param blockp Pointer to the FloatBlock whose values should be printed.
 */
void floatblock_print(struct FloatBlock* blockp) 
{
	block_map((void *)blockp, float_print_callback);
}


/**
 * @brief Entrywise multiplication of two FloatBlocks.
 *
 * @param multiplier The block that stores the factors.
 * @param target The block of other factors, stores the results.
 */
void multiply_floatblock(struct FloatBlock* multiplier,
		struct FloatBlock* target) 
{
	int32_t size = TIMG_BLOCK_SIZE;

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			target->data[y][x] *= multiplier->data[y][x];
		}
	}
}

