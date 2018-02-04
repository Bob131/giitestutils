#include <stdio.h>
#include "test-suite/priv.h"

typedef struct {
  int test_number;
  int n_failed;
} GtuTestSuiteRunData;

static void run_test (GtuTestCase* test_case, GtuTestSuiteRunData* data) {
  char* message = NULL;
  GtuTestResult result;
  GtuPath* path = gtu_test_object_get_path (GTU_TEST_OBJECT (test_case));

  if (_gtu_get_test_mode ()->list_only) {
    fprintf (stdout, "%s\n", gtu_path_to_string (path));
    gtu_path_free (path);
    return;
  }

  {
    bool should_run = true;
    GtuTestMode* test_mode = _gtu_get_test_mode ();
    GList* cursor;

    for (cursor = test_mode->path_selectors;
         cursor != NULL && should_run;
         cursor = cursor->next)
    {
      should_run = gtu_path_has_prefix (path, cursor->data);
    }

    for (cursor = test_mode->path_skippers;
         cursor != NULL && should_run;
         cursor = cursor->next)
    {
      should_run = !gtu_path_has_prefix (path, cursor->data);
    }

    if (should_run) {
      result = _gtu_test_case_run (test_case, &message);

    } else {
      result = GTU_TEST_RESULT_SKIP;
      message = g_strdup ("due to command line args");
    }
  }

  switch (result) {
    case GTU_TEST_RESULT_PASS:
    case GTU_TEST_RESULT_SKIP:
      fprintf (stdout, "ok");
      break;

    case GTU_TEST_RESULT_FAIL:
      fprintf (stdout, "not ok");
      data->n_failed++;
      break;

    default:
      g_assert_not_reached ();
  }

  fprintf (stdout, " %d ", data->test_number++);

  fprintf (stdout, gtu_path_to_string (path));
  gtu_path_free (path);

  fprintf (stdout, " # ");

  switch (result) {
    case GTU_TEST_RESULT_PASS:
      fprintf (stdout, "PASS");
      break;

    case GTU_TEST_RESULT_SKIP:
      fprintf (stdout, "SKIP");
      break;

    case GTU_TEST_RESULT_FAIL:
      fprintf (stdout, "FAIL");
      break;

    default:
      g_assert_not_reached ();
  }

  if (message != NULL) {
    fprintf (stdout, " %s", message);
    g_free (message);
  }

  fprintf (stdout, "\n");
}

int _gtu_test_suite_run_internal (GPtrArray* tests) {
  GtuTestSuiteRunData data;

  if (tests->len == 0) {
    fprintf (stdout, "0..0\n");
    return 0;
  }

  if (!_gtu_get_test_mode ()->list_only)
    fprintf (stdout, "1..%d\n", tests->len);

  data.test_number = 1;
  g_ptr_array_foreach (tests, (GFunc) run_test, &data);

  return data.n_failed;
}
