#include "value.h"
#include <stdlib.h>
#include <assert.h>

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

	assert ((*tinst->type_is)(vtype));
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
