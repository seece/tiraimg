CC=c99
	CFLAGS=-c -g -Wall -Werror-implicit-function-declaration -pedantic -O2 -I../src 
LDFLAGS=-lm -lnetpbm
SOURCES=$(wildcard *.c)
ALL_SOURCES=$(wildcard ../src/*.c) 
ALL_SOURCES:=$(filter-out ../src/main.c, $(ALL_SOURCES))
TEST_OBJECTS=$(SOURCES:.c=.o)
OBJECTS=$(TEST_OBJECTS) $(ALL_SOURCES:.c=.o)
# build the executable to the root dir
EXECUTABLE=../tests

all: $(SOURCES) $(EXECUTABLE)
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY : clean
clean: 
	rm $(EXECUTABLE)
	rm $(TEST_OBJECTS)
	rm temp/testfile.ppm
