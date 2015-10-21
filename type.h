#ifndef _TYPE_H
#define _TYPE_H

#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>

typedef struct _TypeInstance TypeInstance;

typedef void (*TypeInitializer)(void *);
typedef void (*TypeDestructor)(void *);
typedef void (*InstanceInitializer)(void *);
typedef void (*InstanceDestructor)(void *);

typedef uint64_t Type;

struct _TypeInstance {
	/* a value of 0 indicates no parent */
	Type base_type;
	Type type;
	size_t instance_size;
	/* if klass_size == 0, then this is a 
	 * non-instantiable type */
	size_t klass_size;
	const char *name;

	/* the following methods are chained */
	TypeInitializer     type_init;
	TypeDestructor      type_dispose;

	InstanceInitializer instance_init;
	InstanceDestructor  instance_dispose;
};

/**
 * Registers a new type with the type system and returns
 * a unique value identifying that type. Under the hood,
 * this creates a new TypeInstance and stores it somewhere.
 * This TypeInstance is retrievable with the function
 * global_types_get_instance ().
 *
 * There are certain rules as to what a type instance must
 * have defined, according to what kind of type it represents.
 * There are two basic kinds of types you can define:
 *    1. an instantiable type
 *     - This is a type whose instance can be created with
 *       type_instance_instance_new() and destroyed with
 *       type_instance_instance_dispose(). Therefore the
 *       parameters @instance_init and @instance_dispose 
 *       must not be NULL.
 *     - The struct defining the instance must have as its 
 *       first element a pointer to a class (this class must 
 *       "inherit" TypeInstance by containing a (TypeInstance) 
 *       as its first member). Therefore @klass_size must be
 *       at least sizeof(TypeInstance), and @instance_size must 
 *       be at least sizeof(TypeInstance *). Also, @type_init
 *       and @type_dispose must not be NULL.
 *     - The contents of @name must be a _unique_ mnemonic for
 *       the type. It must not be NULL and it must be non-zero
 *       in length.
 *     - Finally, the @base_type must be a type that is
 *       also an instantiable type. The one exception is
 *       TYPE_ANY, which can be used to mean that our type
 *       has no subtype.
 *    2. a non-instantiable type
 *     - This is a type whose instance cannot be created and
 *       destroyed by the dynamic type system. This is usually
 *       used for fundamental types (like char, long, etc).
 *     - @klass_size must be zero
 *     - @instance_size must be non-zero.
 *     - @name is subject to the same rules as above
 *     - @type_init and @type_dispose are ignored for now (TODO)
 *     - @instance_init and @instance_dispose are ignored
 */
Type global_types_register_new (Type base_type,
		size_t instance_size, size_t klass_size,
		const char *name,
		TypeInitializer type_init,
		TypeDestructor type_dispose,
		InstanceInitializer instance_init,
		InstanceDestructor instance_dispose);

TypeInstance *global_types_get_instance (Type type);

void *type_instance_instance_new (Type type);
void type_instance_instance_dispose (void *data);

/* basic types */
#define TYPE_ANY ((Type) 0)
#define TYPE_UCHAR (TYPE_ANY + 1)
#define TYPE_CHAR (TYPE_UCHAR + 1)
#define TYPE_USHORT (TYPE_CHAR + 1)
#define TYPE_SHORT (TYPE_USHORT + 1)
#define TYPE_UINT (TYPE_SHORT + 1)
#define TYPE_INT (TYPE_UINT + 1)
#define TYPE_ULONG (TYPE_INT + 1)
#define TYPE_LONG (TYPE_ULONG + 1)
#define TYPE_BOOL (TYPE_LONG + 1)
#define TYPE_FLOAT (TYPE_BOOL + 1)
#define TYPE_DOUBLE (TYPE_FLOAT + 1)
#define TYPE_POINTER (TYPE_DOUBLE + 1)
#define TYPE_TYPE (TYPE_POINTER + 1)
#define N_BASIC_TYPES (TYPE_TYPE + 1)

#define _DECLARE_TYPE(TypeName, prefix) \
	Type prefix##_get_type (void); \
	bool prefix##_is_type (Type type); \
	static inline TypeName##Class *prefix##_get_class (void *self) {\
		return ((self != NULL && (*(TypeInstance **)self) != NULL && prefix##_is_type ((*(TypeInstance **)self)->type)) ? (*(TypeName##Class **)self) : NULL); \
	} \
	static inline TypeName *prefix##_cast (void *self) {\
		return (prefix##_get_class (self) != NULL ? (TypeName *) self : NULL);\
	}\
	static inline TypeName##Class *prefix##_class_cast (void *self) {\
		return ((self != NULL && prefix##_is_type (((TypeInstance *)self)->type)) ? ((TypeName##Class *)self) : NULL);\
	}

/**
 * for creating new objects, this macro should appear
 * in the .h file
 */
#define DECLARE_TYPE(TypeName, type_prefix) _DECLARE_TYPE(TypeName, type_prefix)

#define _DEFINE_TYPE(TypeName, prefix, BASE_TYPE) \
	static void prefix##_init (TypeName *self);\
	static void __##prefix##_init (void *self) { \
		TypeName *data; \
		data = (*(TypeInstance **)self)->instance_size >= sizeof(*data) ? self : NULL; \
		assert (data != NULL); \
		prefix##_init (data); \
	}\
	static void prefix##_dispose (TypeName *self);\
	static void __##prefix##_dispose (void *self) { \
		TypeName *data; \
		data = (*(TypeInstance **)self)->instance_size >= sizeof(*data) ? self : NULL; \
		assert (data != NULL); \
		prefix##_dispose (data);\
	}\
	static void prefix##_class_init (TypeName##Class *klass);\
	static void __##prefix##_class_init (void *self) { \
		TypeName##Class *data; \
		data = ((TypeInstance *)self)->klass_size >= sizeof(*data) ? self : NULL; \
		assert (data != NULL); \
		prefix##_class_init (data); \
	}\
	static void prefix##_class_dispose (TypeName##Class *klass);\
	static void __##prefix##_class_dispose (void *self) { \
		TypeName##Class *data; \
		data = ((TypeInstance *)self)->klass_size >= sizeof(*data) ? self : NULL; \
		assert (data != NULL); \
		prefix##_class_dispose (data); \
	}\
	Type prefix##_get_type (void) {\
		static Type type = 0;\
		if (type == 0) {\
			type = global_types_register_new (BASE_TYPE, \
					sizeof(TypeName), sizeof(TypeName##Class), \
					"" # TypeName, \
					&__##prefix##_class_init,\
					&__##prefix##_class_dispose,\
					&__##prefix##_init,\
					&__##prefix##_dispose);\
			assert (type != 0); \
		}\
		return type; \
	}\
	bool prefix##_is_type (Type type) {\
		const Type self_type = prefix##_get_type ();\
		TypeInstance *type_inst; \
		while (type > BASE_TYPE) {\
			type_inst = global_types_get_instance (type); \
			assert (type_inst != NULL); \
			if (type_inst->type == self_type) \
				return true;\
			type = type_inst->base_type;\
		}\
		return false;\
	}

/**
 * for creating new objects, this macro should appear
 * in the .c file
 */
#define DEFINE_TYPE(TypeName, type_prefix, base_type) _DEFINE_TYPE(TypeName, type_prefix, base_type)

#endif /* _TYPE_H */
