# This file defines the CC, CFLAGS and LDFLAGS build variables.

CC=c99
CFLAGS=-c -Wall -Werror-implicit-function-declaration -pedantic -O3 -DNDEBUG
LDFLAGS=-lm -lnetpbm
