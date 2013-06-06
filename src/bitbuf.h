#ifndef BITBUF_H_
#define BITBUF_H_

struct BitBuffer {
	/** buffer size in bytes */
	uint32_t size; 
	/** position of the byte that's currently being written */
	uint32_t pos;
	/** sub-byte cursor position, always 0-7*/
	uint8_t bit_pos;
	/** buffer data */
	uint8_t* data;
};

struct BitBuffer* bitbuf_new(uint32_t initial_size);
void bitbuf_del(struct BitBuffer* bitbuf);
void bitbuf_put_bit(struct BitBuffer* buf, int32_t bit);
uint8_t bitbuf_read_bit(struct BitBuffer* buf, uint64_t offset);
#endif
