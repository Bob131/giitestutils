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

static bool should_log (GLogLevelFlags log_level) {
  if (log_level & G_LOG_FLAG_FATAL)
    return true;

  switch (log_level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:
    case G_LOG_LEVEL_CRITICAL:
    case G_LOG_LEVEL_WARNING:
    case G_LOG_LEVEL_MESSAGE:
      return true;

    case G_LOG_LEVEL_INFO:
    case G_LOG_LEVEL_DEBUG:
      return gtu_test_mode_flags_get_flags () & GTU_TEST_MODE_FLAGS_VERBOSE;

    default:
      g_assert_not_reached ();
  }
}

static const char* level_to_string (GLogLevelFlags log_level) {
#define ret(str) {                    \
  if (log_level & G_LOG_FLAG_FATAL) { \
    return "FATAL-" str;              \
  } else {                            \
    return str;                       \
  }                                   \
}

  switch (log_level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:
      ret ("ERROR");

    case G_LOG_LEVEL_CRITICAL:
      ret ("CRITICAL");

    case G_LOG_LEVEL_WARNING:
      ret ("WARNING");

    case G_LOG_LEVEL_MESSAGE:
      ret ("MESSAGE");

    case G_LOG_LEVEL_INFO:
      ret ("INFO");

    case G_LOG_LEVEL_DEBUG:
      ret ("DEBUG");

    default:
      g_assert_not_reached ();
  }

#undef ret
}

static void glib_test_logger (const char* log_domain, GLogLevelFlags log_level,
                              const char* message, void* data)
{
  (void) log_domain;
  (void) data;

  if (should_log (log_level))
    fprintf (stdout, "# %s: %s\n", level_to_string (log_level), message);
}

int _gtu_test_suite_run_internal (GPtrArray* tests) {
  GtuTestSuiteRunData data;
  unsigned log_handler;

  if (tests->len == 0) {
    fprintf (stdout, "0..0\n");
    return 0;
  }

  log_handler = g_log_set_handler (
    NULL,
    G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION,
    glib_test_logger,
    NULL
  );

  fprintf (stdout, "1..%d\n", tests->len);

  data.test_number = 1;
  g_ptr_array_foreach (tests, (GFunc) run_test, &data);

  g_log_remove_handler (NULL, log_handler);

  return data.n_failed;
}
