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
		struct ByteBlock* blockp,
		void (*func)(void* valuep, int32_t x, int32_t y)) 
{
	int32_t size = TIMG_BLOCK_SIZE;

	for (int32_t i=0;i<size*size;i++) {
		int32_t x = i % size;
		int32_t y = i / size; 

		func((void *)&blockp->data[x][y], x, y);
	}
}

void floatblock_init(struct FloatBlock* blockp)
{
	assert(blockp != NULL);
	int32_t size = TIMG_BLOCK_SIZE;

	for (int32_t i=0;i<size*size;i++) {
		blockp->data[i % size][i / size] = -1.0;
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

void byte_print_callback(void* valuep, int32_t x, int32_t y) {
	printf("%3d ", *(uint8_t*)valuep);

	if (x==TIMG_BLOCK_SIZE-1) {
		printf("\n");
	}
}

void byteblock_print(struct ByteBlock* blockp) {
	block_map(blockp, byte_print_callback);
}

