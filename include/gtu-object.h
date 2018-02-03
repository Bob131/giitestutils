#ifndef __GII_TEST_UTILS_OBJECT_H__
#define __GII_TEST_UTILS_OBJECT_H__

/**
 * SECTION:gtu-object
 * @short_description: base test container
 * @title: Test Object
 * @include: gtu.h
 *
 * #GtuTestObject is an abstract ref-counted object, the type from which
 * #GtuTestCase and #GtuTestSuite derive. Instances are floating references on
 * construction; the first call to gtu_test_object_ref() sinks the reference,
 * and additional calls increment the ref-count.
 *
 * Project code may not derive from this type. For building convenience
 * objects, you should derive from #GtuTestCase or #GtuTestSuite instead.
 */

#ifndef __GII_TEST_UTILS_H__
#error "Only <gtu.h> can be included directly."
#endif

G_BEGIN_DECLS

GType gtu_test_object_get_type (void);
#define GTU_TYPE_TEST_OBJECT (gtu_test_object_get_type ())

#define GTU_TEST_OBJECT(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTU_TYPE_TEST_OBJECT, GtuTestObject))

#define GTU_TEST_OBJECT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GTU_TYPE_TEST_OBJECT, GtuTestObjectClass))

#define GTU_IS_TEST_OBJECT(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTU_TYPE_TEST_OBJECT))

/* gtkdoc doesn't handle this correctly */
#ifndef __GTK_DOC_IGNORE__
#define GTU_IS_TEST_OBJECT_CLASS(klass) \
  (G_TYPE_CHECK_CLASSTYPE ((klass), GTU_TYPE_TEST_OBJECT))
#endif

#define GTU_TEST_OBJECT_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTU_TYPE_TEST_OBJECT, GtuTestObjectClass))

/* we can't typedef anonymous structs because gtkdoc won't pick them up */
typedef struct _GtuTestObject      GtuTestObject;
typedef struct _GtuTestObjectClass GtuTestObjectClass;

/**
 * GtuTestObject:
 *
 * Abstract test-containing object.
 */
struct _GtuTestObject {
  /*< private >*/
  GTypeInstance parent_instance;
};

/**
 * GtuTestObjectClass:
 * @finalize: Destructor responsible for cleaning up any resources a
 *            #GtuTestObject instance might hold. If overridden, the new
 *            function must chain up to the parent class finalize.
 *
 * Base class for #GtuTestObject.
 */
struct _GtuTestObjectClass {
  /*< private >*/
  GTypeClass parent_class;
  /*< public >*/
  void (*finalize) (GtuTestObject* self);
};

/**
 * gtu_test_object_ref:
 * @instance: an instance of a #GtuTestObject type.
 *
 * Creates a new owned reference to @instance, either sinking @instance if it's
 * floating or incrementing its reference count.
 *
 * Returns: a new owned reference to @instance.
 */
void* gtu_test_object_ref (void* instance);

/**
 * gtu_test_object_unref:
 * @instance: an instance of a #GtuTestObject type.
 *
 * Destroys a reference to @instance. If @instance's ref-count drops to zero,
 * it will be finalized and freed.
 */
void gtu_test_object_unref (void* instance);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (GtuTestObject, gtu_test_object_unref)

/**
 * gtu_test_object_get_name:
 * @self: an instance of a #GtuTestObject type.
 *
 * Returns the name that identifies @self in the test log.
 *
 * Returns: (transfer none): @self's name.
 */
const char* gtu_test_object_get_name (GtuTestObject* self);

/**
 * gtu_test_object_get_path:
 * @self: an instance of a #GtuTestObject type.
 *
 * Concatenates and returns the names of all @self's parents' names and the
 * name of @self. As the ancestry of @self and its parents changes, this
 * function will return different results.
 *
 * Returns: a #GtuPath instance representing @self. Free this with
 *          gtu_path_free() when it's no longer required.
 */
GtuPath* gtu_test_object_get_path (GtuTestObject* self);

#include "gtu-suite.h"

/**
 * gtu_test_object_get_parent_suite:
 * @self: an instance of a #GtuTestObject type.
 *
 * Returns the test suite to which @self has been added as a child.
 * #GtuTestCase instances must be parented to a #GtuTestSuite to be executed,
 * and #GtuTestSuite instances will typically have an ancestry ending in a root
 * #GtuTestSuite. If @self has not yet been parented, this function returns
 * %NULL instead.
 *
 * Returns: (allow-none) (transfer none): the parent of @self, or %NULL if no
 *                                        parent has been set.
 */
GtuTestSuite* gtu_test_object_get_parent_suite (GtuTestObject* self);

G_END_DECLS

#endif
