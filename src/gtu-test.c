#include "gtu.h"

/* a little demo program */

static void assert_test (void* data) {
  (void) data;
  gtu_assert (TRUE);
  gtu_assert (FALSE);
  g_assert_not_reached ();
}

static void skip_test (void* data) {
  (void) data;
  gtu_skip_if_not_thorough ();
  gtu_skip_if_not_perf ();
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

  g_test_suite_add (suite, gtu_create_test_case ("assert", assert_test,
                                                 NULL, NULL));
  g_test_suite_add (suite, gtu_create_test_case ("skip", skip_test,
                                                 NULL, _destroy));
  g_test_suite_add (suite, gtu_create_test_case ("destroy", destroy_test,
                                                 GINT_TO_POINTER (42),
                                                 _destroy));

  return gtu_run_suite (suite);
}
