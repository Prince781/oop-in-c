#include "object.h"
#include "class.h"

static const struct Class ObjectClass = {
	.size = 8,
	.name = "Object",
	.ctor = NULL,
	.dtor = NULL,
	.differ = NULL
};

const void *Object = &ObjectClass;

