#include "class.h"
#include <stdlib.h>
#include <assert.h>

void *new(const void *_class, ...) {
	const struct Class *class = _class;
	void *p = calloc(1, class->size);

	assert(p != NULL);
	*(const struct Class **)p = _class;

	if (class->ctor) {
		va_list ap;
		va_start(ap, _class);
		p = class->ctor(p, &ap);
		va_end(ap);
	}
	return p;
}

void delete(void *self) {
	const struct Class **cp = self;

	if (self && *cp && (*cp)->dtor)
		self = (*cp)->dtor(self);
	free(self);
}

int differ(const void *self, const void *other) {
	const struct Class *const *cp = self;

	assert(self && *cp && (*cp)->differ);
	return (*cp)->differ(self, other);
}

size_t sizeOf(const void *self) {
	const struct Class *const *cp = self;

	assert(self && *cp);
	return (*cp)->size;
}
