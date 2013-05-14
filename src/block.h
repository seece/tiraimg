#ifndef BLOCK_H_
#define BLOCK_H_

#define TIMG_BLOCK_SIZE 8 

struct Block 
{
	char data[TIMG_BLOCK_SIZE][TIMG_BLOCK_SIZE];
};

void block_init(struct Block* blockp);
struct Block* new_block();

#endif
