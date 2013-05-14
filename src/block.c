#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include "block.h"

void byteblock_init(struct ByteBlock* blockp)
{
	assert(blockp != NULL);
	int32_t size = TIMG_BLOCK_SIZE;

	for (int32_t i=0;i<size*size;i++) {
		blockp->data[i % size][i / size] = 0;
	}

}

struct ByteBlock* new_byteblock() 
{
	struct ByteBlock* blockp = malloc(sizeof(struct ByteBlock));
	byteblock_init(blockp);
	return blockp;
}

void del_byteblock(struct ByteBlock* blockp)
{
	assert(blockp != NULL);
	free(blockp);
}

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

void floatblock_init(struct FloatBlock* blockp)
{
	assert(blockp != NULL);
	int32_t size = TIMG_BLOCK_SIZE;

	for (int32_t y=0;y<size;y++) {
		for (int32_t x=0;x<size;x++) {
			blockp->data[y][x] = 0.0;
		}
	}

}

struct FloatBlock* new_floatblock() 
{
	struct FloatBlock* blockp = malloc(sizeof(struct FloatBlock));
	floatblock_init(blockp);
	return blockp;
}

void del_floatblock(struct FloatBlock* blockp)
{
	assert(blockp != NULL);
	free(blockp);
}

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

/* Some debug helper functions. */
void byte_print_callback(void* valuep, int32_t x, int32_t y) 
{
	printf("%3d ", ((struct ByteBlock*)valuep)->data[y][x]);

	if (x==TIMG_BLOCK_SIZE-1) {
		printf("\n");
	}
}

void float_print_callback(void* valuep, int32_t x, int32_t y) 
{
	printf("%6.2f ", ((struct FloatBlock*)valuep)->data[y][x]);

	if (x==TIMG_BLOCK_SIZE-1) {
		printf("\n");
	}
}

void byteblock_print(struct ByteBlock* blockp) 
{
	block_map((void *)blockp, byte_print_callback);
}

void floatblock_print(struct FloatBlock* blockp) 
{
	block_map((void *)blockp, float_print_callback);
}
