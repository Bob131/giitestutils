#define _POSIX_C_SOURCE 201111L
#include <stdio.h>
#include <string.h>
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

/* we probably don't want to be calling isatty() all the time for something
   trivial, so we wrap it in a once_init */
static bool supports_color (void) {
  /* -1 for unsupported, +1 for supported */
  static volatile gssize supported = 0;

  if (g_once_init_enter (&supported)) {
    gssize result = g_log_writer_supports_color (fileno (stdout)) ? +1 : -1;
    g_once_init_leave (&supported, result);
  }

  return supported > 0 ? true : false;
}

static const char* level_color (GLogLevelFlags level) {
  if (!supports_color ())
    return "";

  if (level & G_LOG_FLAG_FATAL)
    level = G_LOG_LEVEL_ERROR;

  switch (level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:     return "\033[1;31m";
    case G_LOG_LEVEL_CRITICAL:  return "\033[1;35m";
    case G_LOG_LEVEL_WARNING:   return "\033[1;33m";
    case G_LOG_LEVEL_MESSAGE:   return "\033[1;32m";
    case G_LOG_LEVEL_INFO:      return "\033[1;34m";
    case G_LOG_LEVEL_DEBUG:     return "\033[1;36m";
  }

  return "";
}

static const char* color_reset (void) {
  return supports_color () ? "\033[0m" : "";
}

static const char* diagnostic (void) {
  static char* string = NULL;

  if (string == NULL)
    string = g_strdup_printf ("%s#%s ",
                              supports_color () ? "\033[1m" : "",
                              color_reset ());

  return string;
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

/* `message' may be NULL. If non-NULL, it should contain a trailing new line */
static void bail_out (GLogLevelFlags level, const char* message) {
  GtuDebugFlags debug_flags = _gtu_debug_flags_get ();

  fprintf (stdout, "%sBail out!%s", level_color (level), color_reset ());
  if (message != NULL)
    fprintf (stdout, " %s", message);
  else
    fprintf (stdout, "\n");

  if ((level & G_LOG_LEVEL_WARNING &&
       debug_flags & GTU_DEBUG_FLAGS_FATAL_WARNINGS) ||
      (level & G_LOG_LEVEL_CRITICAL &&
       debug_flags & GTU_DEBUG_FLAGS_FATAL_CRITICALS))
  {
    g_abort ();
  }

  /* exit status to signal an error to the automake harness */
  exit (99);
}

static void glib_test_logger (const char* log_domain, GLogLevelFlags log_level,
                              const char* message, void* data)
{
  bool suppress = false;

  (void) data;

  if (_gtu_current_test != NULL &&
      _gtu_test_case_handle_message (_gtu_current_test,
                                     log_domain,
                                     log_level,
                                     message,
                                     (uintptr_t) &glib_test_logger))
  {
    suppress = true;
    if (should_log (G_LOG_LEVEL_INFO))
      goto do_log;
    else
      return;
  }

  if (should_log (log_level)) {
    char *fmtmsg;

do_log:
    fmtmsg = _gtu_log_format_message (log_domain, log_level, message);

    if (suppress) {
      char* smsg = g_strconcat ("Suppressed message: ", fmtmsg, NULL);
      g_free (fmtmsg);
      fmtmsg = _gtu_log_format_message (GTU_LOG_DOMAIN, G_LOG_LEVEL_INFO, smsg);
      g_free (smsg);
    }

    if (log_level & G_LOG_FLAG_FATAL && !suppress) {
      bail_out (log_level, fmtmsg);

    } else {
      _gtu_log_printf (fmtmsg);
      g_free (fmtmsg);
    }
  }
}

static GLogWriterOutput glib_structured_logger (GLogLevelFlags log_level,
                                                const GLogField* fields,
                                                size_t n_fields,
                                                void* data)
{
  size_t i;
  bool suppress = false;

  (void) data;

  if (_gtu_current_test != NULL) {
    const char* message = NULL;
    const char* domain = NULL;

    uintptr_t us = (uintptr_t) &glib_structured_logger;

    for (i = 0; i < n_fields && (message == NULL || domain == NULL); i++) {
      const char** var;

      if (strcmp (fields[i].key, "MESSAGE") == 0) {
        var = &message;
      } else if (strcmp (fields[i].key, "GLIB_DOMAIN")) {
        var = &domain;
      } else {
        continue;
      }

      g_return_val_if_fail (fields[i].length == -1, G_LOG_WRITER_UNHANDLED);
      *var = (const char*) &fields[i].value;
    }

    if (message != NULL && _gtu_test_case_handle_message (_gtu_current_test,
                                                          domain,
                                                          log_level,
                                                          message,
                                                          us))
    {
      suppress = true;
      if (should_log (G_LOG_LEVEL_INFO))
        goto do_log;
      else
        goto ret;
    }
  }

  if (!should_log (log_level))
    goto ret;

do_log:
  _gtu_log_printf ("%s: new structured message:\n",
                   level_to_string (log_level));

  for (i = 0; i < n_fields; i++) {
    char* value = NULL;

    if (fields[i].length == -1)
      value = g_strescape ((const char*) fields[i].value, "\t'\"");

    _gtu_log_printf ("  '%s': %s\n",
                     fields[i].key,
                     value != NULL ? value : "<binary data>");

    if (value != NULL)
      g_free (value);
  }

  if (log_level & G_LOG_FLAG_FATAL && !suppress) {
    bail_out (log_level, NULL);
  }

ret:
  return G_LOG_WRITER_HANDLED;
}

static gboolean fatal_log_handler (const char* log_domain,
                                   GLogLevelFlags log_level,
                                   const char* message,
                                   void* data)
{
  (void) log_level;
  (void) message;
  (void) data;

  /* Fatal logs from us indicate a problem with the user's test suite.
     Otherwise we never want GLib to abort for us, as we handle this
     ourselves. */
  return g_strcmp0 (log_domain, GTU_LOG_DOMAIN) == 0;
}

static void log_empty_test_plan (void) {
  if (has_logged_plan || _gtu_get_test_mode ()->list_only)
    return;

  fprintf (stdout, "1..0 # Skipped: no tests to run\n");
}

static void glib_print_handler (const char* message) {
  int i;
  char buf;

  /* To be TAP-compliant, lines written to stdout that aren't test results must
     be prefixed with '#'. We iterate through `message', printing "# " after
     every newline unless it's a trailing newline. */
  for (i = 0, buf = '\n'; message[i] != '\0'; fputc (buf, stdout), i++) {
    if (buf == '\n')
      fprintf (stdout, diagnostic ());
    buf = message[i];
  }

  if (buf != '\n')
    fprintf (stdout, "\n");
}

static void glib_printerr_handler (const char* message) {
  char* new_message = g_strconcat ("STDERR: ", message, NULL);
  glib_print_handler (new_message);
  g_free (new_message);
}

void _gtu_install_glib_loggers (void) {
  g_set_print_handler (&glib_print_handler);
  g_set_printerr_handler (&glib_printerr_handler);

  g_log_set_default_handler (glib_test_logger, NULL);
  g_log_set_writer_func (glib_structured_logger, NULL, NULL);

  g_test_log_set_fatal_handler (&fatal_log_handler, NULL);

  atexit (&log_empty_test_plan);
}

void _gtu_log_printf (const char* format, ...) {
  va_list args;
  char* message;

  va_start (args, format);
  message = g_strdup_vprintf (format, args);
  glib_print_handler (message);
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

  if (result == GTU_TEST_RESULT_PASS && message == NULL) {
    fprintf (stdout, "\n");
    return;
  }

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

char* _gtu_log_format_message (const char* domain,
                               GLogLevelFlags level,
                               const char* message)
{
  const char *domain_head = "",
             *domain_str  = "",
             *domain_tail = "";

  if (domain != NULL && domain[0] != '\0') {
    domain_head = "(**";
    domain_str = domain;
    domain_tail = ") ";
  }

  return g_strdup_printf ("%s%s%s: %s%s%s%s",
                          level_color (level),
                          level_to_string (level),
                          color_reset (),

                          domain_head,
                          domain_str,
                          domain_tail,

                          message);
}
