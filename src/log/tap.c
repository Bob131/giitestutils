/* Implementation of logio.h */

#include <stdio.h>
#include <stdlib.h>

#include "priv.h"
#include "logio.h"
#include "log-color.h"

static volatile unsigned test_count = 0;
static unsigned expected_tests = 0;
G_LOCK_DEFINE_STATIC (stdout);

static const char* diagnostic (void) {
  return gtu_log_supports_color () ?
    "\033[1m#\033[0m " : /* bolded '# ' */
    "# ";
}

static void log_vprintf (const char* format, va_list args) {
  G_LOCK (stdout);
  vfprintf (stdout, format, args);
  G_UNLOCK (stdout);
}

static void log_printf (const char* format, ...) {
  va_list args;
  va_start (args, format);
  log_vprintf (format, args);
}

static void diag_vprintf (const char* format, va_list args) {
  int i;
  char buf;

  char* message = g_strdup_vprintf (format, args);

  G_LOCK (stdout);

  /* To be TAP-compliant, lines written to stdout that aren't test results must
     be prefixed with '#'. We iterate through `message', printing "# " after
     every newline unless it's a trailing newline. */
  fprintf (stdout, diagnostic ());
  for (i = 0, buf = '\0'; message[i] != '\0'; fputc (buf, stdout), i++) {
    if (buf == '\n')
      fprintf (stdout, "%s  ", diagnostic ()); /* add some indenting */
    buf = message[i];
  }

  if (buf != '\n')
    fputc ('\n', stdout);

  G_UNLOCK (stdout);

  g_free (message);
}

/* Only one test plan can be executed during the lifetime of a process, so we
 * ignore subsequent calls. */
static void log_test_plan (unsigned n_tests, bool disable) {
  static volatile size_t expected_tests__volatile = 0;

  if (g_once_init_enter (&expected_tests__volatile)) {
    if (disable) {
      g_once_init_leave (&expected_tests__volatile, 1);
      return;
    }

    if (n_tests == 0) {
      log_printf ("1..0 # Skipped: no tests to run\n");
      g_once_init_leave (&expected_tests__volatile, 1);
      exit (0);
      g_assert_not_reached ();
    }

    log_printf ("1..%u\n", n_tests);
    expected_tests = n_tests;

    g_once_init_leave (&expected_tests__volatile, n_tests);
  }
}

void gtu_log_disable_test_plan (void) {
  log_test_plan (0, true);
}

void gtu_log_test_plan (unsigned n_tests) {
  log_test_plan (n_tests, false);
}

void gtu_log_diagnostic (const char* format, ...) {
  va_list args;

  g_return_if_fail (format != NULL);

  va_start (args, format);
  diag_vprintf (format, args);
}

void gtu_log_bail_out (bool should_trap, const char* format, ...) {
  va_list args;

  /* pre-empt the atexit() handler */
  gtu_log_disable_test_plan ();

  G_LOCK (stdout);

  fprintf (stdout, "Bail out!");

  if (format != NULL) {
    fputc (' ', stdout);
    va_start (args, format);
    vfprintf (stdout, format, args);
  }

  fputc ('\n', stdout);

  if (should_trap)
    g_abort ();

  /* exit status to signal an error to the automake harness */
  exit (99);
}

static void log_test_result (bool success,
                             const char* test_description,
                             const char* tap_directive,
                             const char* user_directive)
{
  bool has_directive = tap_directive != NULL || user_directive != NULL;
  unsigned prev_test_count = g_atomic_int_add (&test_count, 1);
  log_printf ("%sok %u %s%s%s%s%s\n",
              success ? "" : "not ",
              prev_test_count + 1,
              test_description != NULL ? test_description : "-",
              has_directive ? " # " : "",
              tap_directive != NULL ? tap_directive : "",
              tap_directive != NULL && user_directive != NULL ? " " : "",
              user_directive != NULL ? user_directive : "");
}

void gtu_log_test_success (const char* test_description,
                           const char* directive)
{
  log_test_result (true, test_description, NULL, directive);
}

void gtu_log_test_skipped (const char* test_description,
                           const char* directive)
{
  log_test_result (true, test_description, "SKIP", directive);
}

void gtu_log_test_failed (const char* test_description,
                          const char* directive)
{
  log_test_result (false, test_description, NULL, directive);
}
