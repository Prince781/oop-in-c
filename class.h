#ifndef _CLASS_H
#define _CLASS_H

#include <stddef.h>
#include <stdarg.h>

struct Class {
	size_t size;
	const char *name;
	void *(*ctor)(void *self, va_list *ap);
	void *(*dtor)(void *self);
	void *(*clone)(const void *self);
	int (*differ)(const void *self, const void *other);
};

void *new(const void *_class, ...);

void delete(void *self);

int differ(const void *self, const void *other);

size_t sizeOf(const void *self);

#endif
