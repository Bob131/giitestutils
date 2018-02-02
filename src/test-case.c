#include "gtu-priv.h"

typedef struct {
  GTestCase* test_case;
} GtuTestCasePrivate;

#define PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTU_TYPE_TEST_CASE, GtuTestCasePrivate))
G_DEFINE_TYPE (GtuTestCase, gtu_test_case, GTU_TYPE_TEST_OBJECT)

GTestCase* _gtu_test_case_get_g_case (GtuTestCase* self) {
  return PRIVATE (self)->test_case;
}

static void gtu_test_case_finalize (GtuTestObject* self) {
  GTU_TEST_OBJECT_CLASS (gtu_test_case_parent_class)->finalize (self);
}

static void gtu_test_case_class_init (GtuTestCaseClass* klass) {
  g_type_class_add_private (klass, sizeof (GtuTestCasePrivate));
  GTU_TEST_OBJECT_CLASS (klass)->finalize = gtu_test_case_finalize;
}

static void gtu_test_case_init (GtuTestCase* self) {
  (void) self;
}

GtuTestCase* gtu_test_case_construct (GType type,
                                      const char* name,
                                      GtuTestCaseFunc func,
                                      void* func_target,
                                      GDestroyNotify func_target_destroy)
{
  GtuTestCase* self;

  g_return_val_if_fail (g_type_is_a (type, GTU_TYPE_TEST_CASE), NULL);

  self = (GtuTestCase*) _gtu_test_object_construct (type);

  PRIVATE (self)->test_case =
    _gtu_create_g_test_case (name, func, func_target, func_target_destroy);

  return self;
}

GtuTestCase* gtu_test_case_new (const char* name,
                                GtuTestCaseFunc func,
                                void* func_target,
                                GDestroyNotify func_target_destroy)
{
  return gtu_test_case_construct (
    GTU_TYPE_TEST_CASE, name, func, func_target, func_target_destroy
  );
}
