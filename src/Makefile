CC=c99
CFLAGS=-c -g -Wall -Werror-implicit-function-declaration -pedantic -O2 
LDFLAGS=-lm -lnetpbm
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=../tiraimg

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

clean:
	rm $(OBJECTS)
	rm $(EXECUTABLE)

.PHONY : clean

