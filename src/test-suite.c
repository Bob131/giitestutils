#include "gtu-priv.h"

typedef struct {
  GTestSuite* test_suite;
} GtuTestSuitePrivate;

#define PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTU_TYPE_TEST_SUITE, \
                                GtuTestSuitePrivate))
G_DEFINE_TYPE (GtuTestSuite, gtu_test_suite, GTU_TYPE_TEST_OBJECT)

static bool has_run = false;

void gtu_test_suite_add (GtuTestSuite* self, GtuTestObject* test_object) {
  gtu_test_object_ref (test_object);

  if (GTU_IS_TEST_CASE (test_object)) {
    g_test_suite_add (PRIVATE (self)->test_suite,
                      _gtu_test_case_get_g_case (GTU_TEST_CASE (test_object)));

  } else if (GTU_IS_TEST_SUITE (test_object)) {
    g_test_suite_add_suite (PRIVATE (self)->test_suite,
                            PRIVATE (test_object)->test_suite);

  } else {
    g_critical ("invalid GtuTestObject type %s",
                g_type_name (G_TYPE_FROM_INSTANCE (test_object)));
  }

  gtu_test_object_unref (test_object);
}

int gtu_test_suite_run (GtuTestSuite* self) {
  g_return_val_if_fail (gtu_has_initialized () && !has_run, 1);
  return g_test_run_suite (PRIVATE (self)->test_suite);
}

static void gtu_test_suite_finalize (GtuTestObject* self) {
  GTU_TEST_OBJECT_CLASS (gtu_test_suite_parent_class)->finalize (self);
}

static void gtu_test_suite_class_init (GtuTestSuiteClass* klass) {
  g_type_class_add_private (klass, sizeof (GtuTestSuitePrivate));
  GTU_TEST_OBJECT_CLASS (klass)->finalize = gtu_test_suite_finalize;
}

static void gtu_test_suite_init (GtuTestSuite* self) {
  (void) self;
}

GtuTestSuite* gtu_test_suite_construct (GType type, const char* name) {
  GtuTestSuite* self;

  g_return_val_if_fail (g_type_is_a (type, GTU_TYPE_TEST_SUITE), NULL);

  self = (GtuTestSuite*) _gtu_test_object_construct (type);
  PRIVATE (self)->test_suite = g_test_create_suite (name);

  return self;
}

GtuTestSuite* gtu_test_suite_new (const char* name) {
  return gtu_test_suite_construct (GTU_TYPE_TEST_SUITE, name);
}
