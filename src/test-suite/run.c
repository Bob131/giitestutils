#include <stdio.h>
#include "test-suite/priv.h"

typedef struct {
  int test_number;
  int n_failed;
} GtuTestSuiteRunData;

static void run_test (GtuTestCase* test_case, GtuTestSuiteRunData* data) {
  char* message = NULL;
  GtuTestResult result;
  GtuTestSuite* cursor;
  char* path;

  result = _gtu_test_case_run (test_case, &message);

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

  cursor = gtu_test_object_get_parent_suite (GTU_TEST_OBJECT (test_case));
  path = g_strdup (gtu_test_object_get_name (GTU_TEST_OBJECT (test_case)));

  for (;
       cursor != NULL;
       cursor = gtu_test_object_get_parent_suite (GTU_TEST_OBJECT (cursor)))
  {
    char* new_path = g_strconcat (
      gtu_test_object_get_name (GTU_TEST_OBJECT (cursor)),
      "/",
      path,
      NULL
    );
    g_free (path);
    path = new_path;
  }

  fprintf (stdout, "/%s", path);
  g_free (path);

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

  fprintf (stdout, "1..%d\n", tests->len);

  data.test_number = 1;
  g_ptr_array_foreach (tests, (GFunc) run_test, &data);

  return data.n_failed;
}
