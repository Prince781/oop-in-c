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
	Type type;
	bool is_set;
};

Value *value_new (Type type);

#define value_set(self, tval, val) value_set_typed(self,tval,(Any)val)

#define value_set_value(self, value) value_set_typed(self,value->type,value->val)

void value_set_typed (Value *self, Type vtype, Any any);

Any value_get_typed (Value *self, Type rtype);

#endif
