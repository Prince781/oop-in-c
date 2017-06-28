#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "type.h"

static Type next_type = 0;

/* this is a trie */
struct TypeEntry {
	TypeInstance *type;
	unsigned char symbol;
	/* pointers to next items
	 * TODO: surely we could save space here?
	 */
	struct TypeEntry *next[0x100];
};

static struct TypeEntry types_root = {
	.type = NULL,
	.symbol = 0,
	.next = { NULL }
};

static TypeInstance **types_table = NULL;
static Type types_table_size = 0;

/* for types */
static void type_instance_initialize_chain_up (TypeInstance *tinst, Type type) {
	TypeInstance *base_inst;

	if (type > TYPE_ANY) {
		base_inst = global_types_get_instance (type);

		assert (base_inst != NULL);

		type_instance_initialize_chain_up (tinst, base_inst->base_type);
		(*base_inst->type_init)(tinst);
	}
}

static void type_instance_dispose_chain_down (TypeInstance *tinst, Type type) {
	/* TODO */
}

/* for instances */
static void type_instance_instance_initialize_chain_up (void *data, 
		TypeInstance *type) {
	TypeInstance *base;

	if (type->base_type > TYPE_ANY) {
		base = global_types_get_instance (type->base_type);
		assert (base != NULL);
		assert (base->type != type->type);
		type_instance_instance_initialize_chain_up (data, base);
	}

	if (type->klass_size > 0) {
		assert (type->instance_init != NULL);
		(*type->instance_init)(data);
	}
}

static void type_instance_instance_dispose_chain_down (void *data, 
		TypeInstance *type) {
	TypeInstance *base;

	if (type->klass_size > 0) {
		assert (type->instance_dispose != NULL);
		(*type->instance_dispose)(data);
	}

	if (type->base_type > TYPE_ANY) {
		base = global_types_get_instance (type->base_type);
		assert (base != NULL);
		assert (base->type != type->type);
		type_instance_instance_dispose_chain_down (data, base);
	}
}

static struct TypeEntry *global_types_make_entry (const char *name) {
	struct TypeEntry *root_entry = &types_root;
	struct TypeEntry **entry;
	const unsigned char *ptr;

	assert (name != NULL && *name != '\0');
	entry = &root_entry;
	ptr = name;

	while (*ptr != '\0') {
		entry = &(*entry)->next[*ptr];
		if (*entry == NULL) {
			*entry = calloc(1, sizeof(**entry));
			(*entry)->symbol = *ptr;
		}
		++ptr;
	}

	assert (entry != &root_entry);
	assert (*entry != NULL);
	return *entry;
}

#define TYPE_INSTANCE_INIT(typeval, name, type)\
{ 0, typeval, sizeof(type), 0, #name, NULL, NULL, NULL, NULL, &name##_is_type }

#define TYPE_INSTANCE_INIT_TYPE(typeval, type)\
{ 0, typeval, sizeof(type), 0, #type, NULL, NULL, NULL, NULL, &type##_is_type }

#define is_type_define(prefix, type)\
static bool prefix##_is_type(Type other_type) {\
	return other_type == type;\
}

// #define is_type_define(prefix, type) _is_type_define(prefix, type)

static bool Any_is_type (Type type) { return true; }

is_type_define(uchar, TYPE_UCHAR)
is_type_define(char, TYPE_CHAR)
is_type_define(ushort, TYPE_USHORT)
is_type_define(short, TYPE_SHORT)
is_type_define(uint, TYPE_UINT)
is_type_define(int, TYPE_INT)
is_type_define(ulong, TYPE_ULONG)
is_type_define(long, TYPE_LONG)
is_type_define(bool, TYPE_BOOL)
is_type_define(float, TYPE_FLOAT)
is_type_define(double, TYPE_DOUBLE)
is_type_define(pointer, TYPE_POINTER)
is_type_define(Type, TYPE_TYPE)

static void global_types_register_basic (void) {
	assert (next_type == 0);

	TypeInstance basic_types[N_BASIC_TYPES] = {
		{ 0, TYPE_ANY, 0, 0, "Any", NULL, NULL, NULL, NULL, &Any_is_type },
		TYPE_INSTANCE_INIT(TYPE_UCHAR, uchar, unsigned char),
		TYPE_INSTANCE_INIT_TYPE(TYPE_CHAR, char),
		TYPE_INSTANCE_INIT(TYPE_USHORT, ushort, unsigned short),
		TYPE_INSTANCE_INIT_TYPE(TYPE_SHORT, short),
		TYPE_INSTANCE_INIT(TYPE_UINT, uint, unsigned int),
		TYPE_INSTANCE_INIT_TYPE(TYPE_INT, int),
		TYPE_INSTANCE_INIT(TYPE_ULONG, ulong, unsigned long),
		TYPE_INSTANCE_INIT_TYPE(TYPE_LONG, long),
		TYPE_INSTANCE_INIT_TYPE(TYPE_BOOL, bool),
		TYPE_INSTANCE_INIT_TYPE(TYPE_FLOAT, float),
		TYPE_INSTANCE_INIT_TYPE(TYPE_DOUBLE, double),
		TYPE_INSTANCE_INIT(TYPE_POINTER, pointer, void *),
		TYPE_INSTANCE_INIT_TYPE(TYPE_TYPE, Type)
	};
	struct TypeEntry *entry = NULL;
	Type i;

	types_table_size = N_BASIC_TYPES;
	types_table = calloc(types_table_size, sizeof(*types_table));

	for (i=0; i<N_BASIC_TYPES; ++i) {
		assert (basic_types[i].type == i);
		basic_types[i].name = strdup (basic_types[i].name);

		entry = global_types_make_entry (basic_types[i].name);
		assert (entry->type == NULL);
		entry->type = calloc(1, sizeof(*entry->type));
		memcpy(entry->type, &basic_types[i], sizeof(*entry->type));

		types_table [entry->type->type] = entry->type;
		++next_type;
	}

	assert (next_type == N_BASIC_TYPES);
}

Type global_types_register_new (Type base_type,
		size_t instance_size, size_t klass_size,
		const char *name,
		TypeInitializer type_init,
		TypeDestructor type_dispose,
		InstanceInitializer instance_init,
		InstanceDestructor instance_dispose,
		TypeVerifier type_is) {
	struct TypeEntry *entry;
	TypeInstance *type_inst;

	if (next_type == 0)
		global_types_register_basic ();

	assert (instance_size > 0);
	if (klass_size > 0) {
		/* our type must be instantiable, then */
		assert (instance_size >= sizeof(TypeInstance *));
		assert (klass_size >= sizeof(TypeInstance));
		assert (type_init != NULL);
		assert (type_dispose != NULL);
		assert (instance_init != NULL);
		assert (instance_dispose != NULL);
	}

	assert (type_is != NULL);

	entry = global_types_make_entry (name);

	assert (entry->type == NULL);

	if (klass_size == 0)
		type_inst = calloc (1, sizeof(*type_inst));
	else /* klass_size >= sizeof(TypeInstance) */
		type_inst = calloc (1, klass_size);
	
	assert (type_inst != NULL);

	type_inst->base_type = base_type;
	type_inst->type = next_type++;
	type_inst->instance_size = instance_size;
	type_inst->klass_size = klass_size;
	type_inst->name = strdup (name);

	type_inst->type_init = type_init;
	type_inst->type_dispose = type_dispose;

	type_inst->instance_init = instance_init;
	type_inst->instance_dispose = instance_dispose;

	type_inst->type_is = type_is;

	entry->type = type_inst;
	if (types_table_size <= type_inst->type) {
		uint64_t oldsize = types_table_size;
		types_table_size += oldsize;
		types_table = realloc(types_table, types_table_size * sizeof(*types_table));
		memset(types_table + oldsize, 0, oldsize * sizeof(*types_table));
	}

	assert (type_inst->type < types_table_size);
	types_table [type_inst->type] = type_inst;

	if (klass_size > 0) {
		type_instance_initialize_chain_up (type_inst, type_inst->base_type);
		(*type_inst->type_init)(type_inst);
	}

	return entry->type->type;
}

TypeInstance *global_types_get_instance (Type type) {
	assert (types_table != NULL);
	assert (type < next_type);

	return types_table [type];
}

void *type_instance_instance_new (Type type) {
	TypeInstance *type_inst;
	TypeInstance **ti_ptr;
	void *instance;

	type_inst = global_types_get_instance (type);
	assert (type_inst != NULL);
	assert (type_inst->klass_size >= sizeof(*type_inst));

	instance = calloc(1, type_inst->instance_size);
	ti_ptr = instance;
	*ti_ptr = type_inst;

	/* chain up to base constructor */
	type_instance_instance_initialize_chain_up (instance, type_inst);

	return instance;
}

void type_instance_instance_dispose (void *data) {
	assert (data != NULL);
	assert (*(TypeInstance **)data != NULL);

	type_instance_instance_dispose_chain_down (data, *(TypeInstance **)data);
}
