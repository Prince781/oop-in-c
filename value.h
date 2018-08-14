#ifndef _VALUE_H
#define _VALUE_H
#include "type.h"

/**
 * TODO:
 * this code is of terrible quality
 */

typedef union _ValueAny Any;
typedef struct _Value Value;

union _ValueAny {
	unsigned char _uchar;
	char _char;
	unsigned short _ushort;
	short _short;
	unsigned int _uint;
	int _int;
	unsigned long _ulong;
	long _long;
	bool _bool;
	float _float;
	double _double;
	void *_pointer;
	Type _type;
};

struct _Value {
	union _ValueAny val;
        struct {
            bool is_set : 1;
            Type type   : 63;
        };
};

#define value_create(type_id) ((Value) { .type = type_id, .is_set = false, .val = (Any) 0 })

#define value_init(v) ((Value) { .type = (_Generic((v), \
                unsigned char: TYPE_UCHAR,\
                char: TYPE_CHAR,\
                unsigned short: TYPE_USHORT,\
                short: TYPE_SHORT,\
                unsigned int: TYPE_UINT,\
                int: TYPE_INT,\
                unsigned long: TYPE_ULONG,\
                long: TYPE_LONG,\
                bool: TYPE_BOOL,\
                float: TYPE_FLOAT,\
                double: TYPE_DOUBLE,\
                void *: TYPE_POINTER)), .is_set = true, .val = (Any) (v) })

Value *value_new (Type type);

#define value_set(self, tval, val) value_set_typed(self,tval,(Any)val)

#define value_set_value(self, value) value_set_typed(self,value->type,value->val)

void value_set_typed (Value *self, Type vtype, Any any);

Any value_get_typed (Value *self, Type rtype);

#endif
