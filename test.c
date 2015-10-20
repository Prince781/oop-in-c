#include "object.h"
#include "oop/o-string.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main() {
	Object *o1 = object_new(OBJECT_TYPE, NULL);
	String *str = string_new("<nothing>");
	Object *o2 = object_cast (str);

	char *to_str = object_to_string (o1);
	printf("o1.to_string() = %s\n", to_str);
	printf("o1.refcount = %zu\n", o1->refcount);
	printf("o1.sizeof() = %zu\n", object_sizeof (o1));
	printf("o1 is type %s\n", o1->klass->name);
	free(to_str);

	to_str = object_to_string (o2);
	printf("o2.to_string() = %s\n", to_str);
	printf("o2.refcount = %zu\n", o2->refcount);
	printf("o2.sizeof() = %zu\n", object_sizeof (o2));
	printf("o2 is type %s\n", o2->klass->name);
	free(to_str);

	assert (sizeof(*o1) == object_sizeof(o1));
	assert (sizeof(*str) == object_sizeof(o2));

	object_unref(&o1);
	object_unref(&o2);

	assert (o1 == NULL);
	assert (o2 == NULL);
	printf("o1 and o2 disposed of\n");
}
