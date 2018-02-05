#include <string.h>
#include "test-case/priv.h"

typedef struct {
  GtuTestCaseFunc   func;
  void*             func_target;
  GDestroyNotify    func_target_destroy;
  GPtrArray*        dependencies; /* array of GtuTestObject */
  GtuTestResult     result;
  bool              has_disposed; /* FALSE if we're valid, TRUE if we've been
                                     executed and subsequently freed all
                                     internally held resources. */
} GtuTestCasePrivate;

#define PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTU_TYPE_TEST_CASE, GtuTestCasePrivate))
G_DEFINE_TYPE (GtuTestCase, gtu_test_case, GTU_TYPE_TEST_OBJECT)

static void test_case_dispose (GtuTestCase* self) {
  GtuTestCasePrivate* priv;

  g_assert (GTU_IS_TEST_CASE (self));

  priv = PRIVATE (self);

  if (priv->has_disposed)
    return;

  priv->func = NULL;

  if (priv->func_target_destroy)
    priv->func_target_destroy (priv->func_target);

  priv->func_target = NULL;
  priv->func_target_destroy = NULL;

  if (priv->dependencies)
    g_ptr_array_free (priv->dependencies, true);

  priv->dependencies = NULL;

  priv->has_disposed = true;
}

GtuTestResult _gtu_test_case_run (GtuTestCase* self, char** out_message) {
  char* message = NULL;
  GtuTestCasePrivate* priv;

  g_assert (GTU_IS_TEST_CASE (self));

  priv = PRIVATE (self);

  if (priv->result == GTU_TEST_RESULT_INVALID) {
    priv->result =
      _gtu_test_case_exec_inner (priv->func, priv->func_target, &message);

    test_case_dispose (self);
  }

  if (out_message)
    *out_message = message;

  return priv->result;
}

bool _gtu_test_case_has_run (GtuTestCase* self) {
  g_assert (GTU_IS_TEST_CASE (self));
  return PRIVATE (self)->result != GTU_TEST_RESULT_INVALID;
}

GPtrArray* _gtu_test_case_get_deps (GtuTestCase* self) {
  g_assert (GTU_IS_TEST_CASE (self));
  g_assert (!PRIVATE (self)->has_disposed);
  return PRIVATE (self)->dependencies;
}

void gtu_test_case_add_dependency (GtuTestCase* self,
                                   GtuTestSuiteChild* child)
{
  g_return_if_fail (GTU_IS_TEST_CASE (self));
  g_return_if_fail (GTU_IS_TEST_OBJECT (child));

  /* Defer sanity checking until suite execution, since `self' mightn't be
     parented yet. */

  g_ptr_array_add (PRIVATE (self)->dependencies, gtu_test_object_ref (child));
}

GtuTestCase* gtu_test_case_with_dep (GtuTestCase* self,
                                     GtuTestSuiteChild* child)
{
  gtu_test_case_add_dependency (self, child);
  return self;
}

static void gtu_test_case_finalize (GtuTestObject* self) {
  test_case_dispose (GTU_TEST_CASE (self));
  GTU_TEST_OBJECT_CLASS (gtu_test_case_parent_class)->finalize (self);
}

static void dummy_test_impl (GtuTestCase* self) {
  (void) self;
}

static void gtu_test_case_class_init (GtuTestCaseClass* klass) {
  g_type_class_add_private (klass, sizeof (GtuTestCasePrivate));
  GTU_TEST_OBJECT_CLASS (klass)->finalize = gtu_test_case_finalize;
  klass->test_impl = dummy_test_impl;
}

static void gtu_test_case_init (GtuTestCase* self) {
  PRIVATE (self)->dependencies =
    g_ptr_array_new_with_free_func (gtu_test_object_unref);

  PRIVATE (self)->result = GTU_TEST_RESULT_INVALID;

  PRIVATE (self)->has_disposed = false;
}

static bool _gtu_test_case_construct_internal (GType type,
                                               const char* name,
                                               GtuTestCase** out_self,
                                               GtuTestCasePrivate** out_priv)
{
  g_assert (out_self != NULL);
  g_assert (out_priv != NULL);

  if (!g_type_is_a (type, GTU_TYPE_TEST_CASE) || name == NULL)
    return false;

  *out_self = (GtuTestCase*) _gtu_test_object_construct (type, name);

  if (*out_self == NULL) /* if the base ctor failed */
    return false;

  *out_priv = PRIVATE (*out_self);
  return true;
}

GtuTestCase* gtu_test_case_construct (GType type, const char* name) {
  GtuTestCase* self;
  GtuTestCasePrivate* priv;
  GtuTestCaseClass* klass;

  g_return_val_if_fail (_gtu_path_element_is_valid (name), NULL);

  g_return_val_if_fail (_gtu_test_case_construct_internal (type, name,
                                                           &self, &priv),
                        NULL);

  klass = GTU_TEST_CASE_GET_CLASS (self);
  if (klass->test_impl == dummy_test_impl) {
    g_log (GTU_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
           "GtuTestCase subtype %s fails to override test_impl()",
           g_type_name (type));
    gtu_test_object_unref (self);
    return NULL;
  }

  priv->func = (GtuTestCaseFunc) klass->test_impl;
  priv->func_target = self;
  priv->func_target_destroy = NULL;

  return self;
}

GtuTestCase* gtu_test_case_new (const char* name,
                                GtuTestCaseFunc func,
                                void* func_target,
                                GDestroyNotify func_target_destroy)
{
  GtuTestCase* self;
  GtuTestCasePrivate* priv;

  g_return_val_if_fail (_gtu_path_element_is_valid (name), NULL);
  g_return_val_if_fail (func != NULL, NULL);

  g_return_val_if_fail (_gtu_test_case_construct_internal (GTU_TYPE_TEST_CASE,
                                                           name, &self, &priv),
                        NULL);

  priv->func = func;
  priv->func_target = func_target;
  priv->func_target_destroy = func_target_destroy;

  return self;
}
