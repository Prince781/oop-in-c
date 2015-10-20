#include "o-string.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

DEFINE_TYPE (String, string, OBJECT_TYPE);

static void string_init (String *self) {
	if (self->data == NULL)
		self->data = strdup ("testing string");
	self->length = strlen (self->data);
}

static char *string_to_string_real (Object *object) {
	String *self = string_cast (object);

	assert (self != NULL);
	return strdup (self->data);
}

static void string_dispose (String *self) {
	free(self->data);
	self->data = NULL;
	self->length = 0;
}

static void string_class_init (StringClass *klass) {
	ObjectClass *object_class = object_class_cast (klass);

	assert (object_class != NULL);
	object_class->to_string = string_to_string_real;
}

String *string_new(const char *data) {
	return object_new (STRING_TYPE, NULL);
}
