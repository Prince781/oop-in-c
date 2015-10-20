#ifndef _OBJECT_H
#define _OBJECT_H
#include <stddef.h>
#include <inttypes.h>

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

uint64_t object_get_type(void);

#define ANY_TYPE ((uint64_t) 0ul)

#define OBJECT_TYPE (object_get_type())

Object *object_new (uint64_t type, ...);

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

/**
 * for creating new objects, this macro should appear
 * in the .c file
 */
#define _DEFINE_TYPE(ObjectName, prefix, BASE_TYPE) \
	static void prefix##_##init (ObjectName *self);\
	static void prefix##_##dispose (ObjectName *self);\
	static void prefix##_##class_init (ObjectName##Class *klass);\
	uint64_t prefix##_##get_type (void) {\
		static uint64_t type = 0;\
		if (type == 0) {\
			uint64_t base_type = BASE_TYPE; \
			type = global_types_register_new (#ObjectName, sizeof(ObjectName##Class), \
					sizeof(ObjectName), base_type, \
					(TypeInitializer) &prefix##_class_init); \
			assert (type != 0); \
		}\
		return type; \
	}

#define DEFINE_TYPE(TypeName, type_prefix, base_type) _DEFINE_TYPE(TypeName, type_prefix, base_type)

#endif
