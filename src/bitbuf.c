#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "bitbuf.h"

/**
 * @brief Creates a new BitBuffer.
 *
 * @param initial_size the initial array size in bytes
 *
 * @return new allocated BitBuffer
 */
struct BitBuffer* bitbuf_new(uint32_t initial_size)
{
	assert(initial_size > 0);	
	struct BitBuffer* newbuf = malloc(sizeof(struct BitBuffer));

	newbuf->pos = 0;
	newbuf->bit_pos = 0;
	newbuf->size = initial_size;
	newbuf->data = malloc(initial_size);

	memset(newbuf->data, 0, initial_size);

	return newbuf;
}

/**
 * @brief Deletes an allocated bitbuffer and it's data array.
 *
 * @param bitbuf the buffer to delete
 */
void bitbuf_del(struct BitBuffer* bitbuf)
{
	assert(bitbuf);

	if (bitbuf->data)
		free(bitbuf->data);

	free(bitbuf);
}

/**
 * @brief Writes a byte to the buffer at buf->pos. Doesn't take
 * bit_pos into account. The buffer is expanded if necessary.
 *
 * @param buf buffer to manipulate
 * @param byte byte value to write
 */
void bitbuf_write_byte(struct BitBuffer* buf, uint8_t byte)
{
	if (buf->pos >= buf->size-1) {
		buf->size *= 2;
		buf->data = realloc(buf->data, buf->size);
		assert(buf->data);
	}

	buf->data[buf->pos] = byte;
	buf->pos++;
}

/**
 * @brief Writes a single bit to the buffer advancing the position
 * counter, and expands the buffer size if necessary.
 *
 * @param buf target buffer 
 * @param bit the bit to write
 */
void bitbuf_put_bit(struct BitBuffer* buf, int32_t bit)
{
	assert(bit == 0 || bit == 1);

	buf->data[buf->pos] |= (uint8_t)(bit << (7-buf->bit_pos));
	buf->bit_pos++;

	if (buf->bit_pos == 8) {
		buf->pos++;
		bitbuf_write_byte(buf, 0);		
		buf->pos--;
		buf->bit_pos = 0;
	}
}

/*
void bitbuf_write_bits(struct BitBuffer* buf, int32_t amount, uint8_t data)
{
	if (buf->bit_pos + amount > 7) {
		int32_t left_amount = 7 - buf->bit_pos;

		if (left_amount > 0) {
			// TODO finish this
			//uint8_t left_data = data 
		}

		bitbuf_write_byte(buf, 0);
		// pos has increased by one
		

	}

	int32_t shift = 8 - amount - buf->bit_pos;
	buf->data[buf->pos] |= data<<shift;
}
*/


/**
 * @brief Reads a single bit from a buffer.
 *
 * @param buf the target buffer
 * @param offset the bit offset in bits
 *
 * @return the value of the bit
 */
uint8_t bitbuf_read_bit(struct BitBuffer* buf, uint64_t offset)
{
	assert(buf);
	assert(buf->data);
	assert(offset/8 < buf->size);

	return 0 < (buf->data[offset/8] & (1 << (7 - offset % 8)));

}
