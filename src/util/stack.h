#ifndef STACK_H_
#define STACK_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief A simple pointer stack container.
 */
struct Stack {
	int32_t size; /** stack size in elements */
	int32_t pos; /** the index of the next free member of the stack */
	void** data;
};

struct Stack* stack_new(int32_t initial_size);
void stack_del(struct Stack* stack);
void* stack_pop(struct Stack* stack);
void stack_push(struct Stack* stack, void* pointer);
bool stack_is_empty(struct Stack* stack);
void stack_trim(struct Stack* stack);
#endif
