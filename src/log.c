#include <stdio.h>
#include "log.h"

/* TAP-compliant GLib message handling */

static bool has_logged_plan = false;

static bool should_log (GLogLevelFlags log_level) {
  if (log_level & G_LOG_FLAG_FATAL)
    return true;

  switch (log_level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:
    case G_LOG_LEVEL_CRITICAL:
    case G_LOG_LEVEL_WARNING:
      return true;

    case G_LOG_LEVEL_MESSAGE:
      return !(gtu_test_mode_flags_get_flags () & GTU_TEST_MODE_FLAGS_QUIET);

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
  (void) data;

  if (should_log (log_level)) {
    if (log_level & G_LOG_FLAG_FATAL)
      fprintf (stdout, "Bail out!");
    else
      fprintf (stdout, "#");

    fprintf (stdout, " %s%s%s: %s\n",
             log_domain != NULL ? log_domain : "",
             log_domain != NULL ? "-" : "",
             level_to_string (log_level),
             message);
  }
}

static GLogWriterOutput glib_structured_logger (GLogLevelFlags log_level,
                                                const GLogField* fields,
                                                size_t n_fields,
                                                void* data)
{
  size_t i;

  (void) data;

  if (!should_log (log_level))
    goto ret;

  fprintf (stdout, "# %s: new structured message:\n",
           level_to_string (log_level));

  for (i = 0; i < n_fields; i++) {
    char* value = NULL;

    if (fields[i].length == -1)
      value = g_strescape ((const char*) fields[i].value, "\t'\"");

    fprintf (stdout, "#   '%s': %s\n",
             fields[i].key,
             value != NULL ? value : "<binary data>");

    if (value != NULL)
      g_free (value);
  }

ret:
  return G_LOG_WRITER_HANDLED;
}

static void log_empty_test_plan (void) {
  if (has_logged_plan || _gtu_get_test_mode ()->list_only)
    return;

  fprintf (stdout, "1..0 # Skipped: no tests to run\n");
}

void _gtu_install_glib_loggers (void) {
  g_log_set_default_handler (glib_test_logger, NULL);
  g_log_set_writer_func (glib_structured_logger, NULL, NULL);

  atexit (&log_empty_test_plan);
}

void _gtu_log_printf (const char* format, ...) {
  char* message;
  va_list args;
  int i;
  char buf;

  va_start (args, format);
  message = g_strdup_vprintf (format, args);

  /* To be TAP-compliant, lines written to stdout that aren't test results must
     be prefixed with '#'. We iterate through `message', printing "# " after
     every newline unless it's a trailing newline. */
  for (i = 0, buf = '\n'; message[i] != '\0'; fputc (buf, stdout), i++) {
    if (buf == '\n')
      fprintf (stdout, "# ");
    buf = message[i];
  }

  if (buf != '\n')
    fprintf (stdout, "\n");

  g_free (message);
}

void _gtu_log_test_plan (unsigned n_tests) {
  g_assert (!has_logged_plan);

  if (!_gtu_get_test_mode ()->list_only) {
    if (n_tests == 0)
      log_empty_test_plan ();
    else
      fprintf (stdout, "1..%u\n", n_tests);
  }

  has_logged_plan = true;
}

void _gtu_log_test_result (GtuTestResult result,
                           const char* path,
                           const char* message)
{
  static unsigned test_number = 1;

  g_assert (path != NULL);

  switch (result) {
    case GTU_TEST_RESULT_PASS:
    case GTU_TEST_RESULT_SKIP:
      fprintf (stdout, "ok");
      break;

    case GTU_TEST_RESULT_FAIL:
      fprintf (stdout, "not ok");
      break;

    default:
      g_assert_not_reached ();
  }

  fprintf (stdout, " %d ", test_number++);
  fprintf (stdout, path);
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

  if (message != NULL)
    fprintf (stdout, " %s", message);

  fprintf (stdout, "\n");
}
