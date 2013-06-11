#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include "eks_math.h"
#include "CuTest.h"
#include "util/stack.h"
#include "testHelpers.h"
#include "test_data.h"

static int32_t t_data[4] = {4, 3, 2, 1};

void TestStackCreation(CuTest* tc) 
{
	struct Stack* stack = stack_new(16);
	CuAssertIntEquals(tc, 16, stack->size);
	CuAssertTrue(tc, stack_is_empty(stack));
	stack_del(stack);
}

void TestStackPush(CuTest* tc) 
{
	struct Stack* stack = stack_new(16);

	stack_push(stack, &t_data[0]);
	stack_push(stack, &t_data[1]);
	stack_push(stack, &t_data[2]);
	stack_push(stack, &t_data[3]);

	CuAssertIntEquals(tc, 4, stack->pos);
	CuAssertPtrEquals(tc, &t_data[0], stack->data[0]);
	CuAssertPtrEquals(tc, &t_data[1], stack->data[1]);
	CuAssertPtrEquals(tc, &t_data[2], stack->data[2]);
	CuAssertPtrEquals(tc, &t_data[3], stack->data[3]);

	stack_del(stack);
}

void TestStackPop(CuTest* tc) 
{
	struct Stack* stack = stack_new(16);

	stack_push(stack, &t_data[0]);
	stack_push(stack, &t_data[1]);

	int32_t* popped = stack_pop(stack);

	CuAssertIntEquals(tc, 1, stack->pos);
	CuAssertPtrEquals(tc, &t_data[0], stack->data[0]);
	CuAssertPtrEquals(tc, &t_data[1], popped);

	stack_del(stack);
}

void TestStackTrim(CuTest* tc) 
{
	struct Stack* stack = stack_new(100);

	stack_push(stack, &t_data[0]);
	stack_push(stack, &t_data[1]);
	CuAssertIntEquals(tc, 100, stack->size);
	CuAssertIntEquals(tc, 2, stack->pos);

	stack_trim(stack);

	CuAssertIntEquals(tc, 2, stack->size);

	stack_del(stack);
}

void TestStackExpansion(CuTest* tc) 
{
	struct Stack* stack = stack_new(2);

	stack_push(stack, &t_data[0]);
	stack_push(stack, &t_data[1]);
	CuAssertIntEquals(tc, 2, stack->size);
	stack_push(stack, &t_data[1]);
	CuAssertIntEquals(tc, 4, stack->size);

	stack_del(stack);
}

CuSuite* CuGetStackSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestStackCreation);
	SUITE_ADD_TEST(suite, TestStackPush);
	SUITE_ADD_TEST(suite, TestStackPop);
	SUITE_ADD_TEST(suite, TestStackTrim);
	SUITE_ADD_TEST(suite, TestStackExpansion);

	return suite;
}
