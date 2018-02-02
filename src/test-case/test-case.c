#include <string.h>
#include "test-case/priv.h"

typedef struct {
  GtuTestCaseFunc   func;
  void*             func_target;
  GDestroyNotify    func_target_destroy;
  GtuTestResult     result;
} GtuTestCasePrivate;

#define PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTU_TYPE_TEST_CASE, GtuTestCasePrivate))
G_DEFINE_TYPE (GtuTestCase, gtu_test_case, GTU_TYPE_TEST_OBJECT)

GtuTestResult _gtu_test_case_run (GtuTestCase* self, char** message) {
  GtuTestCasePrivate* priv = PRIVATE (self);

  if (priv->result == GTU_TEST_RESULT_INVALID) {
    priv->result =
      _gtu_test_case_exec_inner (priv->func, priv->func_target, message);

    if (priv->func_target_destroy)
      priv->func_target_destroy (priv->func_target);

    priv->func = NULL;
    priv->func_target = NULL;
    priv->func_target_destroy = NULL;
  }

  return priv->result;
}

static void gtu_test_case_finalize (GtuTestObject* self) {
  GtuTestCasePrivate* priv = PRIVATE (self);

  if (priv->func_target_destroy)
    priv->func_target_destroy (priv->func_target);

  GTU_TEST_OBJECT_CLASS (gtu_test_case_parent_class)->finalize (self);
}

static void gtu_test_case_class_init (GtuTestCaseClass* klass) {
  g_type_class_add_private (klass, sizeof (GtuTestCasePrivate));
  GTU_TEST_OBJECT_CLASS (klass)->finalize = gtu_test_case_finalize;
}

static void gtu_test_case_init (GtuTestCase* self) {
  PRIVATE (self)->result = GTU_TEST_RESULT_INVALID;
}

GtuTestCase* gtu_test_case_construct (GType type,
                                      const char* name,
                                      GtuTestCaseFunc func,
                                      void* func_target,
                                      GDestroyNotify func_target_destroy)
{
  GtuTestCase* self;
  GtuTestCasePrivate priv_init;

  g_return_val_if_fail (g_type_is_a (type, GTU_TYPE_TEST_CASE), NULL);
  g_return_val_if_fail (name != NULL && func != NULL, NULL);

  priv_init = (GtuTestCasePrivate) {
    func,
    func_target,
    func_target_destroy,
    GTU_TEST_RESULT_INVALID
  };

  self = (GtuTestCase*) _gtu_test_object_construct (type, name);
  memcpy (PRIVATE (self), &priv_init, sizeof (GtuTestCasePrivate));

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
