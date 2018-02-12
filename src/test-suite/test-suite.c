#include <string.h>
#include "test-suite/priv.h"

typedef struct {
  GPtrArray*  children;
  GHashTable* child_names;    /* set containing unowned strings */
  GArray*     fatal_messages; /* array of FatalMessage */
} GtuTestSuitePrivate;

#define PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTU_TYPE_TEST_SUITE, \
                                GtuTestSuitePrivate))
G_DEFINE_TYPE (GtuTestSuite, gtu_test_suite, GTU_TYPE_TEST_OBJECT)

typedef struct {
  char* domain;
  GLogLevelFlags flags;
} FatalMessage;

static void fatal_message_dispose (FatalMessage* message) {
  if (message->domain) {
    g_free (message->domain);
    message->domain = NULL;
  }

  message->flags = 0;
}

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

  child = gtu_test_object_ref_sink (test_object);
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

void gtu_test_suite_fail_if_logged (GtuTestSuite* self,
                                    const char* domain,
                                    GLogLevelFlags level)
{
  GtuTestSuitePrivate* priv;
  unsigned index;
  FatalMessage* message;

  g_return_if_fail (GTU_IS_TEST_SUITE (self));
  g_return_if_fail (domain != NULL);
  g_return_if_fail (domain[0] != '\0' && strcmp (domain, GTU_LOG_DOMAIN) != 0);

  if (level == 0)
    level = G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING;

  priv = PRIVATE (self);
  index = priv->fatal_messages->len;
  g_array_set_size (priv->fatal_messages, index + 1);
  message = &g_array_index (priv->fatal_messages, FatalMessage, index);

  message->domain = g_strdup (domain);
  message->flags = level;
}

bool _gtu_test_suite_log_should_fail (GtuTestSuite* self,
                                      const char* log_domain,
                                      GLogLevelFlags log_level)
{
  GtuTestSuitePrivate* priv;
  unsigned i;
  GtuTestSuite* parent;

  g_assert (GTU_IS_TEST_SUITE (self));
  g_assert (log_domain != NULL);

  priv = PRIVATE (self);

  for (i = 0; i < priv->fatal_messages->len; i++) {
    FatalMessage* message =
      &g_array_index (priv->fatal_messages, FatalMessage, i);

    if (strcmp (message->domain, log_domain) != 0)
      continue;

    if ((log_level & message->flags) == 0)
      continue;

    return true;
  }

  parent = gtu_test_object_get_parent_suite (GTU_TEST_OBJECT (self));
  if (parent != NULL)
    return _gtu_test_suite_log_should_fail (parent, log_domain, log_level);

  return false;
}

static void gtu_test_suite_finalize (GtuTestObject* self) {
  GtuTestSuitePrivate* priv = PRIVATE (self);

  g_ptr_array_free (priv->children, true);
  priv->children = NULL;

  g_hash_table_destroy (priv->child_names);
  priv->child_names = NULL;

  g_array_free (priv->fatal_messages, true);
  priv->fatal_messages = NULL;

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
  GtuTestSuitePrivate* priv = PRIVATE (self);

  priv->children = g_ptr_array_new_with_free_func (&gtu_test_object_unref);
  priv->child_names = g_hash_table_new (&g_str_hash, &g_str_equal);

  priv->fatal_messages = g_array_new (false, true, sizeof (FatalMessage));
  g_array_set_clear_func (priv->fatal_messages,
                          (GDestroyNotify) &fatal_message_dispose);

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
