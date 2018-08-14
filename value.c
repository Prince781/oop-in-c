#include "value.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

Value *value_new (Type type) {
    Value *v;

    v = calloc(1, sizeof(*v));
    v->type = type;

    return v;
}

void value_set_typed (Value *self, Type vtype, Any any) {
    TypeInstance *tinst;

    tinst = global_types_get_instance (self->type);

    assert (tinst != NULL);
    assert (tinst->type_is != NULL);

    if (!tinst->type_is(vtype)) {
        TypeInstance *vinst;

        if ((vinst = global_types_get_instance(vtype))) {
            fprintf(stderr, "%s:%d:%s: cannot set value of type `%s' to `%s'\n",
                    __FILE__, __LINE__, __func__, tinst->name, vinst->name);
        } else
            fprintf(stderr, "%s:%d:%s: cannot set value of type `%s' to unknown type\n",
                    __FILE__, __LINE__, __func__, tinst->name);
        abort();
    }
    self->val = any;
    self->is_set = true;
}

Any value_get_typed (Value *self, Type rtype) {
    TypeInstance *rinst;

    assert (self->is_set);
    rinst = global_types_get_instance (rtype);

    assert (rinst != NULL);
    assert (rinst->type_is != NULL);

    /*
     * return type specified (rtype) should
     * be a supertype (or equal to) our
     * value type
     * ie. rtype = R
     *     our value has type T
     *     assert (T <: R); (T is bounded by R)
     */
    assert ((*rinst->type_is)(self->type));

    return self->val;
}
