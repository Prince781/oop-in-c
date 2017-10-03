SOURCES=$(wildcard *.c) $(wildcard */*.c)
OBJECTS=$(SOURCES:%.c=%.o)
CFLAGS=-g -ggdb3
LDFLAGS=
BINARY=oop-test

.PHONY: all clean

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -o $@

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

all: $(BINARY)

clean:
	rm -f $(BINARY)
	rm -f $(OBJECTS)
