#ifndef _OOP_STRING_H
#define _OOP_STRING_H
#include "../object.h"

typedef struct _StringClass StringClass;
typedef struct _String String;

struct _StringClass {
	ObjectClass parent_class;
};

struct _String {
	Object parent_instance;
	char *data;
	size_t length;
};

#define STRING_TYPE (string_get_type ())

DECLARE_TYPE (String, string);

String *string_new(const char *data);

#endif
