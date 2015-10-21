#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main() {
	Object *o1 = object_new(TYPE_OBJECT, NULL);
	TypeInstance *otype = global_types_get_instance (TYPE_OBJECT);

	char *to_str = object_to_string (o1);
	printf("o1.to_string() = %s\n", to_str);
	printf("o1.refcount = %zu\n", o1->refcount);
	printf("o1.sizeof() = %zu\n", object_sizeof (o1));
	printf("o1 is type %s\n", otype->name);
	free(to_str);

	assert (sizeof(*o1) == object_sizeof(o1));

	object_clear_ref (&o1);
	assert (o1 == NULL);
}
