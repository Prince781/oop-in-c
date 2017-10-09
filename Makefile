SOURCES=$(wildcard *.c) $(wildcard */*.c)
OBJECTS=$(SOURCES:%.c=%.o)
CFLAGS=-Wall -Werror -g -ggdb3
LDFLAGS=
BINARY=oop-test

.PHONY: all clean install

all: $(BINARY)

install: $(BINARY)

clean:
	rm -f $(BINARY)
	rm -f $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -o $@

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
