#ifndef _OBJECT_H
#define _OBJECT_H
#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>

typedef struct _ObjectClass ObjectClass;
typedef struct _Object Object;

struct _ObjectClass {
	uint64_t base_type;
	uint64_t type;
	size_t instance_size;
	const char *name;

	/* virtual (chained) methods */
	void (*init)(Object *self);

	/* virtual methods */
	Object *(*ref)(Object *self);
	void (*unref)(Object **selfptr);
	char *(*to_string)(Object *self);

	/* virtual (chained) methods */
	void (*dispose)(Object *self);
};

struct _Object {
	const struct _ObjectClass *klass;
	uint64_t refcount;
};

#define ANY_TYPE ((uint64_t) 0ul)

#define OBJECT_TYPE (object_get_type())

void *object_new (uint64_t type, ...);

Object *object_ref (Object *self);

void object_unref (Object **selfptr);

char *object_to_string (Object *self);

static inline size_t object_sizeof (Object *self) {
	return self->klass->instance_size;
}

typedef void (*TypeInitializer)(void *);

/* NOTE: this should not be called manually */
uint64_t global_types_register_new(const char *name,
		size_t klass_size, size_t instance_size,
		uint64_t base_type,
		TypeInitializer klass_init);


ObjectClass *global_types_get_class_by_name (const char *name);

ObjectClass *global_types_get_class_by_type (uint64_t type);

TypeInitializer global_types_get_type_initializer (uint64_t type);

#define _DECLARE_TYPE(ObjectName, prefix) \
	uint64_t prefix##_##get_type (void); \
	bool prefix##_##is_type (uint64_t type); \
	static inline ObjectName##Class *prefix##_##get_class (void *self) {\
		return ((self != NULL && (*(ObjectClass **)self) != NULL && prefix##_##is_type ((*(ObjectClass **)self)->type)) ? (*(ObjectName##Class **)self) : NULL); \
	} \
	static inline ObjectName *prefix##_##cast (void *self) {\
		return (prefix##_##get_class (self) != NULL ? (ObjectName *) self : NULL);\
	}\
	static inline ObjectName##Class *prefix##_class_cast (void *self) {\
		return ((self != NULL && prefix##_##is_type (((ObjectClass *)self)->type)) ? ((ObjectName##Class *)self) : NULL);\
	}

/**
 * for creating new objects, this macro should appear
 * in the .h file
 */
#define DECLARE_TYPE(TypeName, type_prefix) _DECLARE_TYPE(TypeName, type_prefix)

#define _DEFINE_TYPE(ObjectName, prefix, BASE_TYPE) \
	static void prefix##_##init (ObjectName *self);\
	static void prefix##_##init_prepare (Object *object) {\
		ObjectName *self = prefix##_##cast (object); \
		assert (self != NULL); \
		prefix##_##init (self);\
	}\
	static void prefix##_##dispose (ObjectName *self);\
	static void prefix##_##dispose_prepare (Object *object) {\
		ObjectName *self = prefix##_##cast (object); \
		assert (self != NULL); \
		prefix##_##dispose (self); \
	}\
	static void prefix##_##class_init (ObjectName##Class *klass);\
	uint64_t prefix##_##get_type (void) {\
		static uint64_t type = 0;\
		if (type == 0) {\
			uint64_t base_type = BASE_TYPE; \
			ObjectClass *base_class; \
			type = global_types_register_new (#ObjectName, sizeof(ObjectName##Class), \
					sizeof(ObjectName), base_type, \
					(TypeInitializer) &prefix##_class_init); \
			assert (type != 0); \
			base_class = global_types_get_class_by_type (type);\
			assert (base_class != NULL); \
			base_class->init = &prefix##_##init_prepare; \
			base_class->dispose = &prefix##_##dispose_prepare; \
		}\
		return type; \
	}\
	bool prefix##_##is_type (uint64_t type) {\
		const uint64_t self_type = prefix##_##get_type ();\
		ObjectClass *klass; \
		while (type > BASE_TYPE) {\
			klass = global_types_get_class_by_type (type); \
			assert (klass != NULL); \
			if (klass->type == self_type) \
				return true;\
			type = klass->base_type;\
		}\
		return false;\
	}

/**
 * for creating new objects, this macro should appear
 * in the .c file
 */
#define DEFINE_TYPE(TypeName, type_prefix, base_type) _DEFINE_TYPE(TypeName, type_prefix, base_type)

/* finally, declare the type */
DECLARE_TYPE (Object, object);

#endif
