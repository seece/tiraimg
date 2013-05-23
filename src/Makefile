include ../buildflags.mk

SUBDIRS=$(wildcard */)
SOURCES=$(wildcard *.c) $(wildcard $(addsuffix *.c,$(SUBDIRS)))
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=../tiraimg

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

clean:
	rm $(OBJECTS)
	rm $(EXECUTABLE)

.PHONY : all
.PHONY : clean

