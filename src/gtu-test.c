#include "gtu.h"

/* a little demo program */

#define ASSERT_TEST(suite, test)                                            \
  g_test_suite_add ((suite), gtu_create_test_case (#test, test ## _test,    \
                                                   NULL, NULL))

static void assert_test (void* data) {
  (void) data;
  gtu_assert (TRUE);
  gtu_assert (FALSE);
  g_assert_not_reached ();
}

static void assert_not_reached_test (void* data){
  (void) data;
  gtu_assert_not_reached ();
  g_assert_not_reached ();
}

static void assert_true_test (void* data) {
 (void) data;
 gtu_assert_true (TRUE);
 gtu_assert_true (FALSE);
 g_assert_not_reached ();
}

static void assert_false_test (void* data) {
  (void) data;
  gtu_assert_false (FALSE);
  gtu_assert_false (TRUE);
  g_assert_not_reached ();
}

static void assert_null_test (void* data) {
  (void) data;
  gtu_assert_null (NULL);
  gtu_assert_null ((void*) 34);
  g_assert_not_reached ();
}

static void assert_nonnull_test (void* data) {
  (void) data;
  gtu_assert_nonnull ((void*) 23);
  gtu_assert_nonnull (NULL);
  g_assert_not_reached ();
}

static void skip_test (void* data) {
  (void) data;
  gtu_skip_if_not_thorough ();
  gtu_skip_if_not_perf ();
  gtu_skip_if_not_undefined ();
  gtu_skip_if (TRUE, NULL);
  g_assert_not_reached ();
}

static void destroy_test (void* data) {
  gtu_assert (GPOINTER_TO_INT (data) == 42);
}

static void _destroy (void* data) {
  g_message ("Destroy data %p", data);
}

int main (int argc, char* argv[]) {
  GTestSuite* suite;

  gtu_init (argv, argc);

  suite = g_test_create_suite ("gtu-test");

  ASSERT_TEST (suite, assert);
  ASSERT_TEST (suite, assert_not_reached);
  ASSERT_TEST (suite, assert_true);
  ASSERT_TEST (suite, assert_false);
  ASSERT_TEST (suite, assert_null);
  ASSERT_TEST (suite, assert_nonnull);

  g_test_suite_add (suite, gtu_create_test_case ("skip", skip_test,
                                                 NULL, _destroy));
  g_test_suite_add (suite, gtu_create_test_case ("destroy", destroy_test,
                                                 GINT_TO_POINTER (42),
                                                 _destroy));

  return gtu_run_suite (suite);
}
