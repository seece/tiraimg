CC=c99
CFLAGS=-c -g -Wall -pedantic -O2 -I..
LDFLAGS=
SOURCES=$(wildcard *.c)
ALL_SOURCES=$(wildcard ../*.c) 
ALL_SOURCES:=$(filter-out ../main.c, $(ALL_SOURCES))
TEST_OBJECTS=$(SOURCES:.c=.o)
OBJECTS=$(TEST_OBJECTS) $(ALL_SOURCES:.c=.o)
# build the executable to the root dir
EXECUTABLE=../../test

all: $(SOURCES) $(EXECUTABLE)
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY : clean
clean: 
	rm $(EXECUTABLE)
	rm $(TEST_OBJECTS)
