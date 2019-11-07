#include <string.h>
#include "test-case/priv.h"

G_DEFINE_TYPE (GtuTestCase, gtu_test_case, GTU_TYPE_TEST_OBJECT)

void _gtu_test_case_dispose (GtuTestCase* self) {
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

  if (priv->expected_msgs) {
    g_array_free (priv->expected_msgs, true);
    priv->expected_msgs = NULL;
  }

  priv->has_disposed = true;
}

bool _gtu_test_case_has_run (GtuTestCase* self) {
  g_assert (GTU_IS_TEST_CASE (self));
  return PRIVATE (self)->result != GTU_TEST_RESULT_INVALID;
}

static void gtu_test_case_finalize (GtuTestObject* self) {
  _gtu_test_case_dispose (GTU_TEST_CASE (self));
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
  GtuTestCasePrivate* priv = PRIVATE (self);

  priv->expected_msgs = g_array_new (false, true, sizeof (GtuExpectedMessage));
  g_array_set_clear_func (priv->expected_msgs,
                          (GDestroyNotify) &_gtu_expected_message_dispose);

  priv->result = GTU_TEST_RESULT_INVALID;

  priv->has_disposed = false;
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
  if (klass->test_impl == dummy_test_impl && !GTU_IS_COMPLEX_CASE (self)) {
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
