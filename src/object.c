#include <string.h>
#include "gtu-priv.h"

typedef struct {
  char*         name;
  GtuTestSuite* parent;

  volatile int ref_count;
  volatile int is_floating;
  bool has_finalized;       /* sanity checking */
} GtuTestObjectPrivate;

#define PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTU_TYPE_TEST_OBJECT, \
                                GtuTestObjectPrivate))

static void _gtu_test_object_class_init (void* klass, void* data);
static void _gtu_test_object_init       (GTypeInstance* self, void* klass);
static void _gtu_test_object_finalize   (GtuTestObject* self);

GType gtu_test_object_get_type (void) {
  static volatile gsize type_id__volatile = 0;

  if (g_once_init_enter (&type_id__volatile)) {
    static const GTypeInfo type_info = {
      sizeof (GtuTestObjectClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      &_gtu_test_object_class_init,
      (GClassFinalizeFunc) NULL,
      NULL,                         /* class_data  */
      sizeof (GtuTestObject),
      0,                            /* n_preallocs */
      &_gtu_test_object_init,
      NULL                          /* value_table */
    };

    static const GTypeFundamentalInfo fundamental_info = {
      G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE |
        G_TYPE_FLAG_DEEP_DERIVABLE
    };

    GType type_id = g_type_register_fundamental (
      g_type_fundamental_next (),
      "GtuTestObject",
      &type_info,
      &fundamental_info,
      G_TYPE_FLAG_ABSTRACT
    );

    g_once_init_leave (&type_id__volatile, type_id);
  }

  return type_id__volatile;
}

void* gtu_test_object_ref (void* instance) {
  GtuTestObject* self = GTU_TEST_OBJECT (instance);
  GtuTestObjectPrivate* priv = PRIVATE (self);

  if (!g_atomic_int_compare_and_exchange (&priv->is_floating, 1, 0))
    g_atomic_int_inc (&priv->ref_count);

  return instance;
}

void gtu_test_object_unref (void* instance) {
  GtuTestObject* self = GTU_TEST_OBJECT (instance);
  GtuTestObjectPrivate* priv = PRIVATE (instance);

  if (g_atomic_int_dec_and_test (&priv->ref_count)) {
    GTU_TEST_OBJECT_GET_CLASS (self)->finalize (self);
    g_assert (priv->has_finalized);
    g_type_free_instance ((GTypeInstance*) self);
  }
}

const char* gtu_test_object_get_name (GtuTestObject* self) {
  GtuTestObjectPrivate* priv = PRIVATE (self);
  g_assert (priv->name != NULL);
  return priv->name;
}

GtuPath* gtu_test_object_get_path (GtuTestObject* self) {
  GtuPath* ret = gtu_path_new ();
  gtu_path_append_element (ret, PRIVATE (self)->name);

  if (PRIVATE (self)->parent) {
    GtuPath* parent_path =
      gtu_test_object_get_path (GTU_TEST_OBJECT (PRIVATE (self)->parent));
    gtu_path_prepend_path (ret, parent_path);
    gtu_path_free (parent_path);
  }

  return ret;
}

GtuTestSuite* gtu_test_object_get_parent_suite (GtuTestObject* self) {
  return PRIVATE (self)->parent;
}

void _gtu_test_object_set_parent_suite (GtuTestObject* self,
                                        GtuTestSuite* suite)
{
  g_assert (PRIVATE (self)->parent == NULL);
  PRIVATE (self)->parent = suite;
}

GtuTestObject* _gtu_test_object_construct (GType type, const char* name) {
  GtuTestObject* ret;

  g_assert (g_type_is_a (type, GTU_TYPE_TEST_OBJECT));
  g_assert (name != NULL && strlen (name) > 0);

  ret = (GtuTestObject*) g_type_create_instance (type);
  PRIVATE (ret)->name = g_strdup (name);

  return ret;
}

static void _gtu_test_object_class_init (void* klass, void* data) {
  (void) data;
  g_type_class_add_private (klass, sizeof (GtuTestObjectPrivate));
  GTU_TEST_OBJECT_CLASS (klass)->finalize = _gtu_test_object_finalize;
}

static void _gtu_test_object_init (GTypeInstance* instance, void* klass) {
  GtuTestObject* self = GTU_TEST_OBJECT (instance);
  GtuTestObjectPrivate* priv = PRIVATE (self);

  (void) klass;
  priv->ref_count = 1;
  priv->is_floating = 1;
  priv->has_finalized = false;
}

static void _gtu_test_object_finalize (GtuTestObject* self) {
  GtuTestObjectPrivate* priv = PRIVATE (self);

  g_free (priv->name);
  priv->name = NULL;

  priv->parent = NULL;

  priv->has_finalized = true;
}
