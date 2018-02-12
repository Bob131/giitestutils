#include "gtu.h"

/* a little demo program */

#define ASSERT_TEST(suite, test)                                     \
  gtu_test_suite_add_obj ((suite), gtu_test_case_new (#test,         \
                                                      test ## _test, \
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
  gtu_skip_if_reached (NULL);
  gtu_skip_if_not_thorough ();
  gtu_skip_if_not_perf ();
  gtu_skip_if_not_undefined ();
  gtu_skip_if_fail (FALSE, NULL);
  g_assert_not_reached ();
}

static void destroy_test (void* data) {
  gtu_assert (GPOINTER_TO_INT (data) == 42);
}

static void _destroy (void* data) {
  g_message ("Destroy data %p", data);
}

static void run_first (void* data) {
  (void) data;
  g_message ("run first");
  gtu_assert_not_reached ();
}

static void run_second (void* data) {
  (void) data;
  g_message ("run second");
}

int main (int argc, char* argv[]) {
  GtuTestSuite* suite;

  gtu_init (argv, argc);

  suite = gtu_test_suite_new ("gtu-test");

  ASSERT_TEST (suite, assert);
  ASSERT_TEST (suite, assert_not_reached);
  ASSERT_TEST (suite, assert_true);
  ASSERT_TEST (suite, assert_false);
  ASSERT_TEST (suite, assert_null);
  ASSERT_TEST (suite, assert_nonnull);

  gtu_test_suite_add_obj (suite, gtu_test_case_new ("skip", skip_test,
                                                    NULL, _destroy));
  gtu_test_suite_add_obj (suite, gtu_test_case_new ("destroy", destroy_test,
                                                    GINT_TO_POINTER (42),
                                                    _destroy));

  gtu_test_suite_add_obj (suite, gtu_test_case_new ("first", run_first,
                                                    NULL, NULL));

  gtu_test_suite_add_obj (suite, gtu_test_case_new ("second", run_second,
                                                    NULL, NULL));

  return gtu_test_suite_run (suite);
}
