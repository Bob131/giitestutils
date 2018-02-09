#include <stdio.h>
#include "test-suite/priv.h"
#include "log.h"

typedef struct {
  int test_number;
  int n_failed;
} GtuTestSuiteRunData;

typedef struct {
  GtuPath*      aborted_path;
  GtuTestResult aborted_result;
} GtuTestSuiteAbortData;

static bool result_is_abort (GtuTestResult result) {
  switch (result) {
    case GTU_TEST_RESULT_INVALID:
    case GTU_TEST_RESULT_PASS:
      return false;

    case GTU_TEST_RESULT_SKIP:
    case GTU_TEST_RESULT_FAIL:
      return true;

    default:
      g_assert_not_reached ();
  }
}

static bool abort_filled (GtuTestSuiteAbortData* data) {
  return data->aborted_path != NULL && result_is_abort (data->aborted_result);
}

static void check_deps (GtuTestObject* required,
                        GtuTestCase*   requiree_test)
{
  char *required_path = NULL,
       *requiree_path = NULL,
       *required_path_parent = NULL,
       *requiree_path_parent = NULL;

  GtuTestObject* requiree = GTU_TEST_OBJECT (requiree_test);

  required_path = gtu_test_object_get_path_string (required);
  requiree_path = gtu_test_object_get_path_string (requiree);

  /* requiree_test will have a parent if we're in this function */
  if (gtu_test_object_get_parent_suite (required) == NULL) {
    g_log (GTU_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
           "test `%s' depends on `%s', but the latter has no parent!",
           requiree_path, required_path);
    goto ret;
  }

  required_path_parent = gtu_test_object_get_path_string (
    GTU_TEST_OBJECT (gtu_test_object_get_parent_suite (required))
  );
  requiree_path_parent = gtu_test_object_get_path_string (
    GTU_TEST_OBJECT (gtu_test_object_get_parent_suite (requiree))
  );

  if (gtu_test_object_get_parent_suite (requiree) !=
      gtu_test_object_get_parent_suite (required))
  {
    g_log (GTU_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
           "test `%s' depends on `%s', but they have different parents (`%s' vs `%s')",
           requiree_path,
           required_path,
           requiree_path_parent,
           required_path_parent);
  }

ret:
  if (required_path)        g_free (required_path);
  if (requiree_path)        g_free (requiree_path);
  if (required_path_parent) g_free (required_path_parent);
  if (requiree_path_parent) g_free (requiree_path_parent);
}

static void coalesce_results (GtuTestCase* test_case,
                              GtuTestSuiteAbortData* data)
{
  GtuTestResult test_case_result;

  g_return_if_fail (_gtu_test_case_has_run (test_case));

  if (abort_filled (data))
    return;

  /* grab the result of the prior test execution */
  test_case_result = _gtu_test_case_run (test_case, NULL);

  if (result_is_abort (test_case_result)) {
    data->aborted_path = gtu_test_object_get_path (GTU_TEST_OBJECT (test_case));
    data->aborted_result = test_case_result;
  }
}

static void run_test (GtuTestCase* test_case, GtuTestSuiteRunData* data) {
  char* message = NULL;
  GtuTestResult result = GTU_TEST_RESULT_INVALID;
  GtuPath* path;

  if (_gtu_test_case_has_run (test_case))
    return;

  path = gtu_test_object_get_path (GTU_TEST_OBJECT (test_case));

  if (_gtu_get_test_mode ()->list_only) {
    fprintf (stdout, "%s\n", gtu_path_to_string (path));
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
      GPtrArray* dep_test_cases = g_ptr_array_new ();
      GtuTestSuiteAbortData abort_data = {
        NULL,                   /* aborted_path */
        GTU_TEST_RESULT_INVALID /* aborted_result */
      };

      g_ptr_array_foreach (_gtu_test_case_get_deps (test_case),
                           (GFunc) check_deps, test_case);

      g_ptr_array_foreach (_gtu_test_case_get_deps (test_case),
                           (GFunc) _gtu_test_object_collect_tests,
                           dep_test_cases);

      g_ptr_array_foreach (dep_test_cases, (GFunc) run_test, data);
      g_ptr_array_foreach (dep_test_cases, (GFunc) coalesce_results,
                           &abort_data);

      if (abort_filled (&abort_data)) {
        message =
          g_strdup_printf ("prerequisite test aborted: %s",
                           gtu_path_to_string (abort_data.aborted_path));
        result = abort_data.aborted_result;

      } else {
        result = _gtu_test_case_run (test_case, &message);
      }

      g_ptr_array_free (dep_test_cases, true);

    } else {
      result = GTU_TEST_RESULT_SKIP;
      message = g_strdup ("due to command line args");
    }
  }

  _gtu_log_test_result (result, gtu_path_to_string (path), message);

  if (message != NULL)
    g_free (message);
}

int _gtu_test_suite_run_internal (GPtrArray* tests) {
  GtuTestSuiteRunData data;

  _gtu_log_test_plan (tests->len);

  data.test_number = 1;
  g_ptr_array_foreach (tests, (GFunc) run_test, &data);

  return data.n_failed;
}
