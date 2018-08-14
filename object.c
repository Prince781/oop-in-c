#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "object.h"

DEFINE_TYPE (Object, object, TYPE_ANY);

static void object_init (Object *self) {
    self->refcount = 1;
}

static Object *object_ref_real (Object *self) {
    ++self->refcount;
    return self;
}

static void object_unref_real (Object *self) {
    if (self->refcount == 0)
        return;
    --self->refcount;
    if (self->refcount == 0)
        type_instance_instance_dispose (self);
}

static char *object_to_string_real (Object *self) {
    return strdup((*(TypeInstance **)self)->name);
}

static void object_dispose (Object *self) {
#ifdef OBJECT_DEBUG
    printf("%s: disposing (%s *) %p\n", __func__, 
            (*(TypeInstance **)self)->name, self);
#endif
    assert (self->refcount == 0);
}

static void object_class_init (ObjectClass *klass) {
    /* note: we need not do object_class_cast here,
     * but future objects inheriting may need to
     * ex: we have type Derived, so we'll get a (DerivedClass *)
     *     passed to us. To override the ObjectClass virtual
     *     methods, we call object_class_cast (DerivedClass *)
     *     to get a (ObjectClass *)
     * note: the class's TypeInstance is already defined with
     *       the proper methods and information
     */
    /* virtual methods */
    klass->ref = object_ref_real;
    klass->unref = object_unref_real;
    klass->to_string = object_to_string_real;
}

static void object_class_dispose (ObjectClass *klass) { /* ... */ }

void *object_new(Type type, ...) {
    void *instance;

    instance = type_instance_instance_new (type);
    assert (instance != NULL);

    // TODO: properties / varargs

    return instance;
}

Object *object_ref(Object *self) {
    return object_get_class (self)->ref (self);
}

void object_unref (Object *self) {
    object_get_class (self)->unref (self);
}

void object_clear_ref(Object **selfptr) {
    assert (selfptr != NULL && *selfptr != NULL);
    object_unref (*selfptr);
    *selfptr = NULL;
}

char *object_to_string(Object *self) {
    return object_get_class (self)->to_string (self);
}
