#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "stack.h"

/**
 * @brief Creates a new stack.
 *
 * @param initial_size size of the stack
 *
 * @return pointer to the allocated stack
 */
struct Stack* stack_new(int32_t initial_size)
{
	struct Stack* stackp = calloc(1, sizeof(struct Stack));
	stackp->data = calloc(initial_size, sizeof(void*));
	stackp->size = initial_size;
	stackp->pos = 0;

	return stackp;
}

/**
 * @brief Deletes a stack. The allocated data buffer is freed.
 *
 * @param stack the stack to free
 */
void stack_del(struct Stack* stack)
{
	assert(stack);
	assert(stack->data);

	free(stack->data);
	free(stack);
}

/**
 * @brief Doubles the stack size.
 *
 * @param stack target stack
 */
static void stack_expand(struct Stack* stack)
{
	stack->size*=2;
	stack->data = realloc(stack->data, stack->size);
}

/**
 * @brief Pops an item off the stack.
 *
 * @param stack the stack containing the item
 *
 * @return the popped item
 */
void* stack_pop(struct Stack* stack)
{
	if (stack_is_empty(stack)) {
		return NULL;
	}

	stack->pos--;
	return stack->data[stack->pos];
}

/**
 * @brief Pushes an item to the stack. The stack size is incremented
 * if necessary.
 *
 * @param stack target stack
 * @param pointer target item
 */
void stack_push(struct Stack* stack, void* pointer)
{
	assert(stack);

	if (stack->pos >= stack->size) {
		stack_expand(stack);
	}

	stack->data[stack->pos] = pointer;
	stack->pos++;
}

/**
 * @brief Checks if the given stack is empty.
 *
 * @param stack target stack
 *
 * @return true if empty, false if not
 */
bool stack_is_empty(struct Stack* stack)
{
	assert(stack);

	if (stack->pos == 0) {
		return true;
	}

	return false;
}

/**
 * @brief Shrinks the stack to the lowest size possible without
 * discarding elements currently on the stack.
 *
 * @param stack target stack
 */
void stack_trim(struct Stack* stack)
{
	assert(stack);
	assert(stack->data);

	if (stack->pos == 0) {
		return;
	}

	stack->size = stack->pos;
	stack->data = realloc(stack->data, stack->pos);
}

/**
 * @brief Empties the stack.
 *
 * @param stack stack to empty
 */
void stack_clear(struct Stack* stack)
{
	stack->pos = 0;
}
