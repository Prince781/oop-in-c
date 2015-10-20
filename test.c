#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main() {
	Object *o1 = object_new(OBJECT_TYPE, NULL);
	Object *o2 = object_new(OBJECT_TYPE, NULL);

	char *to_str = object_to_string (o1);
	printf("o1.to_string() = %s\n", to_str);
	printf("o1.refcount = %zu\n", o1->refcount);
	printf("o1.sizeof() = %zu\n", object_sizeof(o1));

	assert (sizeof(*o1) == object_sizeof(o1));

	assert (o1->klass->type == o2->klass->type);

	object_unref(&o1);
	object_unref(&o2);

	assert (o1 == NULL);
	assert (o2 == NULL);
	printf("o1 and o2 disposed of\n");

	free(to_str);
}
