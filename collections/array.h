#ifndef _ARRAY_H
#define _ARRAY_H
#include "../object.h"
#include "../value.h"

typedef struct _ArrayClass ArrayClass;
typedef struct _Array Array;

struct _ArrayClass {
    ObjectClass parent_class;
};

struct _Array {
    Object parent_instance;

    /* public instance fields */
    Type etype;	/* element type */

    size_t capacity;
    size_t length;	/* number of elements */
    Any *data;
};

#define TYPE_ARRAY (array_get_type ())

Array *array_new(Type elem_type);

void array_add(Array *self, Value *item);

void array_resize(Array *self, size_t newsize);

Value array_get(const Array *self, size_t idx);

Type array_get_elemtype(const Array *self);

DECLARE_TYPE(Array, array);

#endif
