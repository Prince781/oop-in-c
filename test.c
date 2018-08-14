#include "object.h"
#include "collections/array.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    Object *o1 = object_new(TYPE_OBJECT, NULL);
    ObjectClass *oclass = object_get_class (o1);
    TypeInstance *otype = (TypeInstance *) oclass;

    char *to_str = object_to_string (o1);
    printf("o1.to_string() = %s\n", to_str);
    printf("o1.refcount = %zu\n", o1->refcount);
    printf("o1.sizeof() = %zu\n", object_sizeof (o1));
    printf("o1 is type %s\n", otype->name);
    free(to_str);

    assert (sizeof(*o1) == object_sizeof(o1));

    object_clear_ref (&o1);
    assert (o1 == NULL);

    // test array
    Array *array_int = array_new (TYPE_INT);
    Array *array_obj = array_new (TYPE_OBJECT);

    to_str = object_to_string (object_cast (array_int));
    printf ("array_int is a %s\n", to_str);
    free(to_str);
    to_str = object_to_string (object_cast (array_obj));
    printf ("array_obj is a %s\n", to_str);
    free(to_str);

    // test array more
    for (int i = 0; i < 32; ++i)
        array_add(array_int, &value_init(i));

    assert(array_int->length > 0);

    for (size_t i = 0; i < array_int->length; ++i) {
        Value val = array_get(array_int, i);
        printf ("array_int[%zu] = %d\n", i, value_get_typed(&val, TYPE_INT)._int);
    }

    // test values
    Value *val = value_new(TYPE_INT);
    value_set_typed(val, TYPE_INT, (Any) 100);
    // value_set_typed(val, TYPE_DOUBLE, (Any) 0.3);

    assert (val->is_set);

    printf ("value(%s) = %d\n", global_types_get_instance(val->type)->name,
            val->val._int);

    return 0;
}
