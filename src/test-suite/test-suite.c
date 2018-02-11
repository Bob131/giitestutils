#include <stdio.h>
#include "test-suite/priv.h"

typedef struct {
  GPtrArray*  children;
  GHashTable* child_names; /* set containing unowned strings */
} GtuTestSuitePrivate;

#define PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTU_TYPE_TEST_SUITE, \
                                GtuTestSuitePrivate))
G_DEFINE_TYPE (GtuTestSuite, gtu_test_suite, GTU_TYPE_TEST_OBJECT)

GtuTestSuiteChild* gtu_test_suite_add (GtuTestSuite* self,
                                       GtuTestObject* test_object)
{
  GtuTestSuitePrivate* priv;
  GtuTestObject* child;

  g_return_val_if_fail (GTU_IS_TEST_SUITE  (self),        NULL);
  g_return_val_if_fail (GTU_IS_TEST_OBJECT (test_object), NULL);
  g_return_val_if_fail (gtu_test_object_get_parent_suite (test_object) == NULL,
                        NULL);

  priv = PRIVATE (self);

  /* ght_add returns TRUE if the insertion is unique */
  g_return_val_if_fail (
    g_hash_table_add (priv->child_names,
                      (void*) gtu_test_object_get_name (test_object)),
    NULL
  );

  child = gtu_test_object_ref (test_object);
  g_ptr_array_add (priv->children, child);
  _gtu_test_object_set_parent_suite (child, self);

  return child;
}

void _gtu_test_object_collect_tests (GtuTestObject* object, GPtrArray* tests) {
  g_assert (GTU_IS_TEST_OBJECT (object));

  if (GTU_IS_TEST_CASE (object)) {
    g_ptr_array_add (tests, GTU_TEST_CASE (object));

  } else if (GTU_IS_TEST_SUITE (object)) {
    g_ptr_array_foreach (PRIVATE (object)->children,
                         (GFunc) _gtu_test_object_collect_tests, tests);

  } else {
    g_log (GTU_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
           "Unknown GtuTestObject type: %s",
           g_type_name (G_TYPE_FROM_INSTANCE (object)));
  }
}

int gtu_test_suite_run (GtuTestSuite* self) {
  GPtrArray* tests;
  int ret;

  static bool has_run = false;

  /* It doesn't matter what we return here, since our logging system will
     exit() with the correct thing. */
  g_return_val_if_fail (gtu_has_initialized (),   1);
  g_return_val_if_fail (!has_run,                 1);
  g_return_val_if_fail (GTU_IS_TEST_SUITE (self), 1);

  _gtu_test_object_sink (self);

  tests = g_ptr_array_new ();
  _gtu_test_object_collect_tests (GTU_TEST_OBJECT (self), tests);

  ret = _gtu_test_suite_run_internal (tests);

  g_ptr_array_free (tests, true);
  gtu_test_object_unref (GTU_TEST_OBJECT (self));

  has_run = true;

  return ret;
}

static void gtu_test_suite_finalize (GtuTestObject* self) {
  GtuTestSuitePrivate* priv = PRIVATE (self);

  g_ptr_array_free (priv->children, true);
  priv->children = NULL;

  g_hash_table_destroy (priv->child_names);
  priv->child_names = NULL;

  GTU_TEST_OBJECT_CLASS (gtu_test_suite_parent_class)->finalize (self);
}

static void gtu_test_suite_class_init (GtuTestSuiteClass* klass) {
  g_type_class_add_private (klass, sizeof (GtuTestSuitePrivate));
  GTU_TEST_OBJECT_CLASS (klass)->finalize = gtu_test_suite_finalize;
}

static void propagate_signal (GtuTestSuite* self, void* data) {
  unsigned i;
  GtuTestSuitePrivate* priv;

  (void) data;

  priv = PRIVATE (self);
  for (i = 0; i < priv->children->len; i++)
    _gtu_test_object_emit_ancestry_signal (
      GTU_TEST_OBJECT (priv->children->pdata[i])
    );
}

static void gtu_test_suite_init (GtuTestSuite* self) {
  PRIVATE (self)->children =
    g_ptr_array_new_with_free_func (gtu_test_object_unref);

  PRIVATE (self)->child_names = g_hash_table_new (&g_str_hash, &g_str_equal);

  g_signal_connect (self,
                    "ancestry-changed",
                    (GCallback) &propagate_signal, NULL);
}

GtuTestSuite* gtu_test_suite_construct (GType type, const char* name) {
  g_return_val_if_fail (g_type_is_a (type, GTU_TYPE_TEST_SUITE), NULL);
  g_return_val_if_fail (_gtu_path_element_is_valid (name), NULL);
  return (GtuTestSuite*) _gtu_test_object_construct (type, name);
}

GtuTestSuite* gtu_test_suite_new (const char* name) {
  return gtu_test_suite_construct (GTU_TYPE_TEST_SUITE, name);
}
