#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "stack.h"

struct Stack* stack_new(int32_t initial_size)
{
	struct Stack* stackp = calloc(1, sizeof(struct Stack));
	stackp->data = calloc(initial_size, sizeof(void*));
	stackp->size = initial_size;
	stackp->pos = 0;

	return stackp;
}

void stack_del(struct Stack* stack)
{
	assert(stack);
	assert(stack->data);

	free(stack->data);
	free(stack);
}

static void stack_expand(struct Stack* stack)
{
	stack->size*=2;
	stack->data = realloc(stack->data, stack->size);
}

void* stack_pop(struct Stack* stack)
{
	if (stack_is_empty(stack)) {
		return NULL;
	}

	stack->pos--;
	return stack->data[stack->pos];
}

void stack_push(struct Stack* stack, void* pointer)
{
	assert(stack);

	if (stack->pos >= stack->size) {
		stack_expand(stack);
	}

	stack->data[stack->pos] = pointer;
	stack->pos++;
}

bool stack_is_empty(struct Stack* stack)
{
	assert(stack);

	if (stack->pos == 0) {
		return true;
	}

	return false;
}

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
