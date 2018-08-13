OBJDIR=obj
SOURCES=$(wildcard *.c) $(wildcard */*.c)
OBJECTS=$(SOURCES:%.c=$(OBJDIR)/%.o)
CFLAGS=-Wall -Werror -g3 -ggdb3 -Wextra -Wformat=2 -Wformat-truncation -Wno-unused-parameter
LDFLAGS=
BINARY=oop-test

.PHONY: all clean install

all: $(BINARY)

install: $(BINARY)

clean:
	rm -f $(BINARY)
	rm -f $(OBJECTS)
	rm -rf $(OBJDIR)

$(OBJDIR):
	mkdir $@

$(OBJDIR)/%.o: %.c
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -o $@

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
