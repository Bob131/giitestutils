#include <stdio.h>
#include "test-suite/priv.h"
#include "log/logio.h"

static void run_test (GtuTestCase* test_case, int* n_failed) {
  char* message = NULL;
  GtuTestResult result = GTU_TEST_RESULT_INVALID;
  const GtuPath* path;

  if (_gtu_test_case_has_run (test_case))
    return;

  path = gtu_test_object_get_path (GTU_TEST_OBJECT (test_case));

  if (_gtu_get_test_mode ()->list_only) {
    fprintf (stdout, "%s\n", gtu_path_to_string (path));
    return;
  }

  if (_gtu_path_should_run (path)) {
    result = _gtu_test_case_run (test_case, &message);

  } else {
    result = GTU_TEST_RESULT_SKIP;
    message = g_strdup ("due to command line args");
  }

  switch (result) {
    case GTU_TEST_RESULT_PASS:
      gtu_log_test_success (gtu_path_to_string (path), message);
      break;

    case GTU_TEST_RESULT_SKIP:
      gtu_log_test_skipped (gtu_path_to_string (path), message);
      break;

    case GTU_TEST_RESULT_FAIL:
      gtu_log_test_failed (gtu_path_to_string (path), message);
      (*n_failed)++;
      break;

    default:
      g_assert_not_reached ();
  }

  if (message != NULL)
    g_free (message);
}

static void count_tests (GtuTestCase* test_case, unsigned* n_tests) {
  if (GTU_IS_COMPLEX_CASE (test_case)) {
    *n_tests += _gtu_complex_case_get_length (GTU_COMPLEX_CASE (test_case));
  }

  (*n_tests)++;
}

int _gtu_test_suite_run_internal (GPtrArray* tests) {
  int n_failed = 0;
  unsigned n_tests = 0;

  g_ptr_array_foreach (tests, (GFunc) &count_tests, &n_tests);
  gtu_log_test_plan (n_tests);

  g_ptr_array_foreach (tests, (GFunc) &run_test, &n_failed);
  return n_failed;
}
