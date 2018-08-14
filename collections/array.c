#include "array.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

DEFINE_TYPE (Array, array, TYPE_OBJECT);

static void array_init (Array *self) {
    self->capacity = 10;
    self->data = calloc(self->capacity, sizeof(*self->data));
}

static void array_dispose (Array *self) {
    /* TODO: free */
}

static char *array_class_to_string_real (Object *self) {
    static char buf[1024];
    Array *_self;
    TypeInstance *self_tinst;
    TypeInstance *elem_tinst;

    _self = array_cast (self);
    self_tinst = *(TypeInstance **) self;
    elem_tinst = global_types_get_instance (_self->etype);

    assert (elem_tinst != NULL);
    snprintf (buf, sizeof(buf) - 1, "%s<%s>",
            self_tinst->name, elem_tinst->name);
    return strdup (buf);
}

static void array_class_init (ArrayClass *klass) {
    ObjectClass *object_class;

    object_class = (ObjectClass *) klass;
    object_class->to_string = array_class_to_string_real;
}

static void array_class_dispose (ArrayClass *klass) { }

Array *array_new(Type elem_type) {
    Array *array;

    array = object_new (TYPE_ARRAY, NULL);
    array->etype = elem_type;

    return array;
}

void array_add(Array *self, Value *item) {
    while (self->length >= self->capacity)
        array_resize (self, self->length * 2);

    Value tmpval = value_create(self->etype);
    value_set_value (&tmpval, item);
    self->data[self->length++] = tmpval.val;
}

void array_resize(Array *self, size_t newsize) {
    assert (newsize > self->capacity);

    self->data = realloc(self->data, newsize * sizeof(*self->data));
    memset(self->data + self->length, 0,
            (newsize - self->capacity) * sizeof(*self->data));
    self->capacity = newsize;
}

Value array_get(const Array *self, size_t idx) {
    assert (idx < self->length);
    // return self->data[idx];
    return (Value){
        .val = self->data[idx],
        .is_set = true, 
        .type = self->etype
    };
}

Type array_get_elemtype(const Array *self) {
    return self->etype;
}
