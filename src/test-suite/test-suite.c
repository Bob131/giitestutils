#include <stdio.h>
#include "test-suite/priv.h"

typedef struct {
  GPtrArray* children;
} GtuTestSuitePrivate;

#define PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTU_TYPE_TEST_SUITE, \
                                GtuTestSuitePrivate))
G_DEFINE_TYPE (GtuTestSuite, gtu_test_suite, GTU_TYPE_TEST_OBJECT)

GtuTestSuiteChild* gtu_test_suite_add (GtuTestSuite* self,
                                       GtuTestObject* test_object)
{
  GtuTestObject* child;

  g_return_val_if_fail (gtu_test_object_get_parent_suite (test_object) == NULL,
                        NULL);

  child = gtu_test_object_ref (test_object);
  g_ptr_array_add (PRIVATE (self)->children, child);
  _gtu_test_object_set_parent_suite (child, self);

  return child;
}

static void collect_tests (GtuTestObject* object, GPtrArray* tests) {
  if (GTU_IS_TEST_CASE (object)) {
    g_ptr_array_add (tests, GTU_TEST_CASE (object));

  } else if (GTU_IS_TEST_SUITE (object)) {
    g_ptr_array_foreach (PRIVATE (object)->children,
                         (GFunc) collect_tests, tests);

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

  g_return_val_if_fail (gtu_has_initialized () && !has_run, TEST_ERROR);

  tests = g_ptr_array_new ();
  collect_tests (GTU_TEST_OBJECT (self), tests);

  ret = _gtu_test_suite_run_internal (tests);

  g_ptr_array_free (tests, true);
  gtu_test_object_unref (GTU_TEST_OBJECT (self));

  has_run = true;

  return ret;
}

static void gtu_test_suite_finalize (GtuTestObject* self) {
  g_ptr_array_free (PRIVATE (self)->children, true);
  PRIVATE (self)->children = NULL;

  GTU_TEST_OBJECT_CLASS (gtu_test_suite_parent_class)->finalize (self);
}

static void gtu_test_suite_class_init (GtuTestSuiteClass* klass) {
  g_type_class_add_private (klass, sizeof (GtuTestSuitePrivate));
  GTU_TEST_OBJECT_CLASS (klass)->finalize = gtu_test_suite_finalize;
}

static void gtu_test_suite_init (GtuTestSuite* self) {
  PRIVATE (self)->children =
    g_ptr_array_new_with_free_func (gtu_test_object_unref);
}

GtuTestSuite* gtu_test_suite_construct (GType type, const char* name) {
  g_return_val_if_fail (g_type_is_a (type, GTU_TYPE_TEST_SUITE), NULL);
  return (GtuTestSuite*) _gtu_test_object_construct (type, name);
}

GtuTestSuite* gtu_test_suite_new (const char* name) {
  return gtu_test_suite_construct (GTU_TYPE_TEST_SUITE, name);
}
