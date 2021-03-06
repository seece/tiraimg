/**
 * ByteBlock and FloatBlock manipulation functions.
 */


#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <assert.h>
#include <stdint.h>
#include "eks_math.h"
#include "block.h"

typedef void (*map_func_t)(void* valuep, int32_t x, int32_t y);

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
struct ByteBlock* byteblock_new() 
{
	struct ByteBlock* blockp = malloc(sizeof(struct ByteBlock));
	byteblock_init(blockp);
	return blockp;
}


/**
 * @brief Frees memory of a ByteBlock allocated from the heap.
 *
 * @param blockp Pointer to the ByteBlock.
 */
void byteblock_del(struct ByteBlock* blockp)
{
	assert(blockp != NULL);
	free(blockp);
}


/**
 * @brief Executes a callback function on every member of a FloatBlock
 * or ByteBlock.
 *
 * @param blockp Pointer to the block to iterate over.
 * @param func The callback function to call on each value.
 */
static void block_map(void* blockp, map_func_t func)
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
struct FloatBlock* floatblock_new() 
{
	struct FloatBlock* blockp = malloc(sizeof(struct FloatBlock));
	floatblock_init(blockp);
	return blockp;
}


/**
 * @brief Frees memory of a FloatBlock allocated from the heap.
 *
 * @param blockp Pointer to the FloatBlock to be freed.
 */
void floatblock_del(struct FloatBlock* blockp)
{
	assert(blockp != NULL);
	free(blockp);
}


/**
 * @brief Adds a certain floating point value to the each value of a ByteBlock.
 *
 * @param input The FloatBlock to read the original values from.
 * @param bias_value The value to add to all values of this FloatBlock.
 * @param output Result will be stored in this FloatBlock.
 */
void byteblock_add(
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
 * @brief Adds a certain floating point value to the each value of a FloatBlock.
 *
 * @param input The FloatBlock to read the original values from.
 * @param bias_value The value to add to all values of this FloatBlock.
 * @param output Result will be stored in this FloatBlock.
 */
void floatblock_add(
		struct FloatBlock const* input,
	       	float bias_value, 
		struct FloatBlock* output) 
{
	int size = TIMG_BLOCK_SIZE;

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			float val =input->data[y][x];
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
void byteblock_to_float(const struct ByteBlock *input, 
		struct FloatBlock* output)
{
	int size = TIMG_BLOCK_SIZE;

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			output->data[y][x] = (float)input->data[y][x];
		}
	}
}


/**
 * @brief Converts a FloatBlock to a ByteBlock. Output is rounded to
 * the nearest integer.
 *
 * @param input original FloatBlock
 * @param output rounded output block
 */
void floatblock_to_byte(const struct FloatBlock *input, 
		struct ByteBlock* output)
{
	int size = TIMG_BLOCK_SIZE;

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			float val = input->data[y][x];

			if (val < 0) {
				fprintf(stderr, "Warning: float value %f < 0 in byte conv!\n", val);
			}

			if (val > 255) {
				fprintf(stderr, "Warning: float value %f > 255 in byte conv!\n", val);
			}

			output->data[y][x] = MIN(255, MAX(0, round(val)));
		}
	}
}

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
void floatblock_multiply(struct FloatBlock* multiplier,
		struct FloatBlock* target) 
{
	int32_t size = TIMG_BLOCK_SIZE;

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			target->data[y][x] *= multiplier->data[y][x];
		}
	}
}


/**
 * @brief Maps a block index (range: [0, 64[) to a unique xy-coordinates.
 * Used for reorganizing blocks to a zigzag pattern and vice-versa.
 *
 * @param pos the input index
 * @param xp pointer where the calcluated x-coordinate is saved
 * @param yp pointer where the calculated y-coordinate is saved
 */
static void get_zigzag_pos(int32_t pos, int32_t* xp, int32_t* yp)
{
	int32_t i = pos;

	// we just mirror the values to get the offsets for
	// the second half of the block
	if (pos >= 36) {
		i = 27-(pos-36);
	}

	int32_t phase = floor((1.0+sqrt(8*i+1))/2.0);
	int32_t edge = (phase * (phase-1))/2.0;
	int32_t index = i - edge;

	int32_t x = phase - index - 1;
	int32_t y = index;

	if (pos >= 36) {
		x = 7-x;
		y = 7-y;
	}

	if ((phase % 2) == 0) {
		int32_t temp = x;
		x = y;
		y = temp;
	}

	*xp = x;
	*yp = y;

}

/**
 * @brief Reorganizes the block contents to a zigzag-pattern for
 * better compression. 
 *
 * @param input source data
 * @param output output block, shouldn't be same as input
 */
void byteblock_pack(struct ByteBlock* input, struct ByteBlock* output)
{
	for (int i=0;i<64;i++) {
		int32_t x = i % 8;
		int32_t y = i / 8;
		int32_t x2, y2;	
		get_zigzag_pos(i, &x2, &y2);

		output->data[y][x] = input->data[y2][x2];
	}
}


/**
 * @brief Unpacks a zigzag pattern byteblock.
 *
 * @param input the reorganized block
 * @param output destination for data, shouldn't be same as input
 */
void byteblock_unpack(struct ByteBlock* input, struct ByteBlock* output)
{
	for (int i=0;i<64;i++) {
		int32_t x = i%8;
		int32_t y = i/8;
		int32_t x2, y2;	
		get_zigzag_pos(i, &x2, &y2);

		output->data[y2][x2] = input->data[y][x];
	}
}
