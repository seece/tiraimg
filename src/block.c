#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include "block.h"

void block_init(struct Block* blockp)
{
	assert(blockp != NULL);
	int size = TIMG_BLOCK_SIZE;

	for (int i=0;i<size*size;i++) {
		blockp->data[i % size][i / size] = 0;
	}
}

struct Block* new_block() 
{
	struct Block* blockp = malloc(sizeof(struct Block));
	block_init(blockp);
	return blockp;
}

struct Block* del_block(struct Block* blockp)
{
	free(blockp);
}
