# This file defines the CC, CFLAGS and LDFLAGS build variables.

CC=c99
CFLAGS=-c -g -Wall -Werror-implicit-function-declaration -pedantic -O2  
LDFLAGS=-lm -lnetpbm
