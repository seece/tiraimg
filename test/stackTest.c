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

void TestStackCreation(CuTest* tc) 
{
	struct Stack* stack = stack_new(16);
	CuAssertIntEquals(tc, 16, stack->size);
	stack_del(stack);
}

CuSuite* CuGetStackSuite(void) 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestStackCreation);

	return suite;
}
