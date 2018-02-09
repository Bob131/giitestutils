#include <string.h>
#include "gtu-priv.h"
#include <gobject/gvaluecollector.h>

typedef struct {
  char*         name;
  GtuTestSuite* parent;
  GtuPath*      path;

  volatile int ref_count;
  volatile int is_floating;
  bool has_finalized;       /* sanity checking */
} GtuTestObjectPrivate;

#define PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTU_TYPE_TEST_OBJECT, \
                                GtuTestObjectPrivate))

enum {
  ANCESTRY_CHANGED,
  LAST_SIGNAL
};

static unsigned long object_signals[LAST_SIGNAL] = {0};

static void _gtu_test_object_class_init (void* klass, void* data);
static void _gtu_test_object_init       (GTypeInstance* self, void* klass);
static void _gtu_test_object_finalize   (GtuTestObject* self);

static inline void** _value_ptr (const GValue* value) {
  return (void**) &value->data[0].v_pointer;
}

static void _value_init (GValue* value) {
  *_value_ptr (value) = NULL;
}

static void _value_free (GValue* value) {
  g_return_if_fail (GTU_IS_TEST_OBJECT (*_value_ptr (value)));
  gtu_test_object_unref (*_value_ptr (value));
}

static void _value_copy (const GValue* src_value, GValue* dest_value) {
  g_return_if_fail (GTU_IS_TEST_OBJECT (*_value_ptr (src_value)));
  *_value_ptr (dest_value) = gtu_test_object_ref (*_value_ptr (src_value));
}

static void* _value_peek (const GValue* value) {
  return *_value_ptr (value);
}

static char* _value_collect (GValue* value,
                             unsigned n_collect_values,
                             GTypeCValue* collect_values,
                             unsigned collect_flags)
{
  (void) n_collect_values;
  (void) collect_flags;

  if (collect_values[0].v_pointer) {
    GtuTestObject* object = collect_values[0].v_pointer;

    if (object->parent_instance.g_class == NULL) {
      return g_strdup_printf (
        "invalid unclassed object pointer for value type `%s'",
        G_VALUE_TYPE_NAME (value)
      );

    } else if (!GTU_IS_TEST_OBJECT (object)) {
      return g_strdup ("invalid GtuTestObject pointer");
    }

    *_value_ptr (value) = gtu_test_object_ref (collect_values[0].v_pointer);

  } else {
    *_value_ptr (value) = NULL;
  }

  return NULL;
}

static char* _value_lcopy (const GValue* value,
                           unsigned n_collect_values,
                           GTypeCValue* collect_values,
                           unsigned collect_flags)
{
  GtuTestObject** object_ptr;

  (void) n_collect_values;

  object_ptr = collect_values[0].v_pointer;

  if (object_ptr == NULL)
    return g_strdup_printf ("value location for `%s' passed as NULL",
                            G_VALUE_TYPE_NAME (value));

  if (*_value_ptr (value) == NULL) {
    *object_ptr = NULL;
  } else if (collect_flags & G_VALUE_NOCOPY_CONTENTS) {
    *object_ptr = *_value_ptr (value);
  } else {
    *object_ptr = gtu_test_object_ref (*_value_ptr (value));
  }

  return NULL;
}

GType gtu_test_object_get_type (void) {
  static volatile gsize type_id__volatile = 0;

  if (g_once_init_enter (&type_id__volatile)) {
    static const GTypeValueTable value_table = {
      &_value_init,
      &_value_free,
      &_value_copy,
      &_value_peek,
      "p",
      &_value_collect,
      "p",
      &_value_lcopy
    };

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
      &value_table
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
  GtuTestObjectPrivate* priv;

  g_return_val_if_fail (GTU_IS_TEST_OBJECT (instance), NULL);

  priv = PRIVATE (instance);

  if (!g_atomic_int_compare_and_exchange (&priv->is_floating, 1, 0))
    g_atomic_int_inc (&priv->ref_count);

  return instance;
}

void gtu_test_object_unref (void* instance) {
  GtuTestObject* self;
  GtuTestObjectPrivate* priv;

  g_return_if_fail (GTU_IS_TEST_OBJECT (instance));

  self = GTU_TEST_OBJECT (instance);
  priv = PRIVATE (instance);

  if (g_atomic_int_dec_and_test (&priv->ref_count)) {
    GTU_TEST_OBJECT_GET_CLASS (self)->finalize (self);
    g_assert (priv->has_finalized);
    g_type_free_instance ((GTypeInstance*) self);
  }
}

void* gtu_test_object_sink (void* instance) {
  GtuTestObjectPrivate* priv;

  g_return_val_if_fail (GTU_IS_TEST_OBJECT (instance), NULL);

  priv = PRIVATE (instance);
  g_atomic_int_set (&priv->is_floating, 0);

  return instance;
}

const char* gtu_test_object_get_name (GtuTestObject* self) {
  GtuTestObjectPrivate* priv;

  g_return_val_if_fail (GTU_IS_TEST_OBJECT (self), NULL);

  priv = PRIVATE (self);

  g_assert (priv->name != NULL);
  return priv->name;
}

GtuPath* gtu_test_object_get_path (GtuTestObject* self) {
  GtuTestObjectPrivate* priv;

  g_return_val_if_fail (GTU_IS_TEST_OBJECT (self), NULL);

  priv = PRIVATE (self);

  if (priv->path == NULL) {
    priv->path = gtu_path_new ();
    gtu_path_append_element (priv->path, priv->name);

    if (priv->parent)
      gtu_path_prepend_path (
        priv->path,
        gtu_test_object_get_path (GTU_TEST_OBJECT (priv->parent))
      );
  }

  return priv->path;
}

char* gtu_test_object_get_path_string (GtuTestObject* self) {
  g_return_val_if_fail (GTU_IS_TEST_OBJECT (self), NULL);
  return g_strdup (gtu_path_to_string (gtu_test_object_get_path (self)));
}

GtuTestSuite* gtu_test_object_get_parent_suite (GtuTestObject* self) {
  g_return_val_if_fail (GTU_IS_TEST_OBJECT (self), NULL);
  return PRIVATE (self)->parent;
}

void _gtu_test_object_emit_ancestry_signal (GtuTestObject* self) {
  g_signal_emit (self, object_signals[ANCESTRY_CHANGED], 0);
}

void _gtu_test_object_set_parent_suite (GtuTestObject* self,
                                        GtuTestSuite* suite)
{
  g_assert (GTU_IS_TEST_OBJECT (self));
  g_assert (GTU_IS_TEST_SUITE (suite));
  g_assert (PRIVATE (self)->parent == NULL);

  PRIVATE (self)->parent = suite;
  _gtu_test_object_emit_ancestry_signal (self);
}

GtuTestObject* _gtu_test_object_construct (GType type, const char* name) {
  GtuTestObject* ret;

  g_assert (g_type_is_a (type, GTU_TYPE_TEST_OBJECT));
  g_assert (_gtu_path_element_is_valid (name));

  ret = (GtuTestObject*) g_type_create_instance (type);
  PRIVATE (ret)->name = g_strdup (name);

  return ret;
}

static void _gtu_test_object_class_init (void* klass, void* data) {
  (void) data;

  g_type_class_add_private (klass, sizeof (GtuTestObjectPrivate));
  GTU_TEST_OBJECT_CLASS (klass)->finalize = _gtu_test_object_finalize;

  object_signals[ANCESTRY_CHANGED] =
    g_signal_new_class_handler ("ancestry-changed",
                                G_TYPE_FROM_CLASS (klass),
                                G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                NULL, /* class handler */
                                NULL, /* accumulator */
                                NULL, /* accumulator data */
                                NULL, /* C marshaller */
                                G_TYPE_NONE,
                                0     /* n_params */);
}

static void dirty_path (GtuTestObject* self, void* data) {
  GtuTestObjectPrivate* priv;

  (void) data;

  priv = PRIVATE (self);
  if (priv->path) {
    gtu_path_free (priv->path);
    priv->path = NULL;
  }
}

static void _gtu_test_object_init (GTypeInstance* instance, void* klass) {
  GtuTestObject* self = GTU_TEST_OBJECT (instance);
  GtuTestObjectPrivate* priv = PRIVATE (self);

  (void) klass;
  priv->ref_count = 1;
  priv->is_floating = 1;
  priv->has_finalized = false;

  g_signal_connect (instance,
                    "ancestry-changed",
                    (GCallback) &dirty_path, NULL);
}

static void _gtu_test_object_finalize (GtuTestObject* self) {
  GtuTestObjectPrivate* priv = PRIVATE (self);

  g_free (priv->name);
  priv->name = NULL;

  priv->parent = NULL;

  if (priv->path) {
    gtu_path_free (priv->path);
    priv->path = NULL;
  }

  /* The type annotation in GLib is wrong: this function is for all GTypes with
     signals, not just GObject. */
  g_signal_handlers_destroy (self);

  priv->has_finalized = true;
}
