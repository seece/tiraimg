#ifndef STACK_H_
#define STACK_H_

#include <stdint.h>
#include <stdbool.h>
struct Stack {
	int32_t size;
	int32_t pos;
	void** data;
};

struct Stack* stack_new(int32_t initial_size);
void stack_del(struct Stack* stack);
void* stack_pop(struct Stack* stack);
void stack_push(struct Stack* stack, void* pointer);
bool stack_is_empty(struct Stack* stack);
void stack_trim(struct Stack* stack);
#endif
