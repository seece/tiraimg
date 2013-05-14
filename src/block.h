#ifndef BLOCK_H_
#define BLOCK_H_

#include <stdint.h>

#define TIMG_BLOCK_SIZE 8 

struct ByteBlock 
{
	uint8_t data[TIMG_BLOCK_SIZE][TIMG_BLOCK_SIZE];
};

struct FloatBlock 
{
	float data[TIMG_BLOCK_SIZE][TIMG_BLOCK_SIZE];
};

void byteblock_init(struct ByteBlock* blockp);
struct ByteBlock* new_byteblock();
void del_byteblock(struct ByteBlock* blockp);
void byteblock_print(struct ByteBlock* blockp);

void floatblock_init(struct FloatBlock* blockp);
struct FloatBlock* new_floatblock();
void del_floatblock(struct FloatBlock* blockp);
void floatblock_print(struct FloatBlock* blockp);



#endif
