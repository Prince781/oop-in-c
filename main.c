#include <stdio.h>
#include "object.h"
#include "class.h"

int main() {
	struct Object *o = new(Object);

	printf("sizeOf(object) = %zu\n", sizeOf(o));
	printf("object name = %s\n",
			(*(const struct Class **)o)->name);
	return 0;
}
