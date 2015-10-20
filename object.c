#include "object.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static uint64_t class_slots = 0ul;
static ObjectClass **defined_classes = NULL;
static TypeInitializer *type_initializers = NULL;
static uint64_t next_type = 2ul;

/* this is a trie */
struct TypeEntry {
	ObjectClass *klass;
	unsigned char symbol;
	/* pointers to next items
	 * TODO: surely we could save space here?
	 */
	struct TypeEntry *next[0x100];
};

static struct TypeEntry types_root = {
	.klass = NULL,
	.symbol = 0,
	.next = { NULL }
};

static void klass_init_chain_up (uint64_t base_type, ObjectClass *klass) {
	TypeInitializer klass_init;
	ObjectClass *parent_class;

	if (base_type > 0) {
		klass_init = global_types_get_type_initializer (base_type);
		parent_class = global_types_get_class_by_type (base_type);
		
		assert (klass_init != NULL && parent_class != NULL);

		klass_init_chain_up (parent_class->base_type, klass);
		(*klass_init)(klass);
	}
}

/**
 * Registers a new type with the dynamic type
 * system.
 * TODO: make this thread-safe
 * params:
 * @name - name of type (not class), like "Object" for ObjectClass
 * @klass_size - size of class (like sizeof(ObjectClass))
 * @instance_size - size of instance (like sizeof(Object))
 * @base_type - the super type of this type (like ANY_TYPE for Object)
 * @klass_init - a function to initialize this class definition
 *     (NOTE: chaining is done automatically, so the class is first initialized
 *            by its base initializer, then so on until (*klass_init) is called)
 */
uint64_t global_types_register_new(const char *name,
		size_t klass_size, size_t instance_size,
		uint64_t base_type,
		TypeInitializer klass_init) {
	struct TypeEntry *types_root_ptr = &types_root;
	struct TypeEntry **entry = &types_root_ptr;
	const unsigned char *ptr;

	assert (strcmp (name, "") != 0);
	assert (klass_size >= sizeof(ObjectClass));
	assert (instance_size >= sizeof(Object));

	ptr = name;
	while (*ptr != '\0') {
		entry = &(*entry)->next[*ptr];
		if (*entry == NULL) {
			*entry = calloc(1, sizeof(**entry));
			(*entry)->symbol = *ptr;
		}
		++ptr;
	}

	assert (entry != &types_root_ptr && *entry != NULL);
	assert ((*entry)->klass == NULL);

	if (defined_classes == NULL || next_type >= class_slots) {
		uint64_t old_slots = class_slots;
		class_slots = next_type * 2;
		defined_classes = realloc(defined_classes, class_slots * sizeof(*defined_classes));
		memset(defined_classes + old_slots, 0, (class_slots - old_slots) * sizeof(*defined_classes));
		type_initializers = realloc(type_initializers, class_slots * sizeof(*type_initializers));
		memset(type_initializers + old_slots, 0, (class_slots - old_slots) * sizeof(*type_initializers));
	}

	(*entry)->klass = calloc(1, klass_size);
	(*entry)->klass->base_type = base_type;
	(*entry)->klass->type = next_type++;
	(*entry)->klass->instance_size = instance_size;
	(*entry)->klass->name = strdup(name);

	defined_classes [(*entry)->klass->type] = (*entry)->klass;
	type_initializers [(*entry)->klass->type] = klass_init;

	klass_init_chain_up (base_type, (*entry)->klass);
	assert (klass_init != NULL);
	(*klass_init)((*entry)->klass);

	return (*entry)->klass->type;
}

ObjectClass *global_types_get_class_by_name (const char *name) {
	struct TypeEntry *entry = &types_root;
	const unsigned char *ptr;

	assert (strcmp(name, "") != 0);

	ptr = name;
	while (*ptr != '\0' && entry != NULL) {
		entry = entry->next[*ptr];
	}

	if (entry == NULL)
		return NULL;

	return entry->klass;
}

ObjectClass *global_types_get_class_by_type (uint64_t type) {
	ObjectClass *klass;

	assert (type < class_slots);
	klass = defined_classes [type];
	assert (klass->type == type);

	return klass;
}

TypeInitializer global_types_get_type_initializer (uint64_t type) {
	TypeInitializer klass_init;

	assert (type < class_slots);
	klass_init = type_initializers [type];
	assert (klass_init != NULL);

	return klass_init;
}

DEFINE_TYPE (Object, object, ANY_TYPE);

static void object_init (Object *self) {
	self->refcount = 1;
}

static Object *object_ref_real(Object *self) {
	++self->refcount;
	return self;
}

static void instance_chain_down_dispose(uint64_t type, Object *object) {
	ObjectClass *klass;

	klass = global_types_get_class_by_type (type);
	assert (klass != NULL);

	klass->dispose (object);
	if (klass->base_type > 0)
		instance_chain_down_dispose (klass->base_type, object);
}

static void object_unref_real(Object **selfptr) {
	assert (selfptr != NULL);

	if (*selfptr == NULL || (*selfptr)->refcount == 0)
		return;
	--(*selfptr)->refcount;
	Object *self = *selfptr;
	*selfptr = NULL;
	if (self->refcount == 0)
		instance_chain_down_dispose (self->klass->type, self);
}

static char *object_to_string_real (Object *self) {
	return strdup(self->klass->name);
}

static void object_dispose (Object *self) {
#ifdef OBJECT_DEBUG
	printf("%s: disposing %p\n", __func__, self);
	assert (self->refcount == 0);
#endif
	free(self);
}

static void object_class_init (ObjectClass *klass) {
	/* note: we need not do object_class_cast here,
	 * but future objects inheriting may need to
	 * ex: we have type Derived, so we'll get a (DerivedClass *)
	 *     passed to us. To override the ObjectClass virtual
	 *     methods, we call object_class_cast (DerivedClass *)
	 *     to get a (ObjectClass *)
	 */
	/* virtual methods */
	klass->ref = object_ref_real;
	klass->unref = object_unref_real;
	klass->to_string = object_to_string_real;
	/* note: mandatory chained methods, like
	 * init() and dispose(), are automatically
	 * defined after this function call
	 */
}

static void instance_chain_up_init(uint64_t type, Object *object) {
	ObjectClass *klass;

	klass = global_types_get_class_by_type (type);
	assert (klass != NULL);

	if (klass->base_type > 0)
		instance_chain_up_init (klass->base_type, object);

	assert(klass->instance_size <= object->klass->instance_size);
	klass->init (object);
}

void *object_new(uint64_t type, ...) {
	ObjectClass *klass;
	Object *instance;

	klass = global_types_get_class_by_type (type);
	assert (klass != NULL);

	instance = calloc(1, klass->instance_size);
	instance->klass = klass;

	/* chain up to base constructor */
	instance_chain_up_init(klass->type, instance);

	// TODO: varargs

	return instance;
}

Object *object_ref(Object *self) {
	return object_get_class (self)->ref (self);
}

void object_unref(Object **selfptr) {
	object_get_class (*selfptr)->unref (selfptr);
}

char *object_to_string(Object *self) {
	return object_get_class (self)->to_string (self);
}
