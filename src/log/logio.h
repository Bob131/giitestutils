#ifndef __GII_TEST_UTILS_LOGIO_H__
#define __GII_TEST_UTILS_LOGIO_H__

#include <stdbool.h>
#include <glib.h>

/**
 * Generic functions for writing to the test log. We do our best to be thread
 * safe in here, since there's a fair bit of global state.
 */

/**
 * gtu_log_test_plan:
 * @n_tests: number of tests expected to run.
 *
 * Writes a test plan to the log, notifying the harness of the number of test
 * cases to expect. If @n_tests is zero, print an empty test plan and exit.
 *
 * The result of calling this function multiple times is undefined.
 */
void gtu_log_test_plan (unsigned n_tests);

/**
 * gtu_log_disable_test_plan:
 *
 * Don't print a test plan when gtu_log_test_plan() is called.
 */
void gtu_log_disable_test_plan (void);

/**
 * gtu_log_diagnostic:
 * @format: printf format string.
 * @...:    values to be formatted.
 *
 * Constructs a formatted string and writes the results to the test log.
 */
void gtu_log_diagnostic (const char* format, ...);

/**
 * gtu_log_bail_out:
 * @should_trap: %TRUE if we should abort, %FALSE to call exit()
 * @format:      (allow-none): printf format string.
 * @...:         values to be formatted.
 *
 * Signals to the test harness that a fatal error has occurred and exits,
 * optionally printing a descriptive message alongside it.
 */
void gtu_log_bail_out (bool should_trap, const char* format, ...)
  G_GNUC_PRINTF (2, 3)
  G_GNUC_NORETURN;

/**
 * gtu_log_test_success:
 * @text_description: (allow-none): description of the test.
 * @directive:        (allow-none): additional information about the success.
 *
 * Indicate to the test harness that a test has passed successfully.
 *
 * Neither @test_description nor @directive shall contain newline characters.
 */
void gtu_log_test_success (const char* test_description, const char* directive);

/**
 * gtu_log_test_skipped:
 * @text_description: (allow-none): description of the test.
 * @directive:        (allow-none): additional information about the skip.
 *
 * Indicate to the test harness that a test was skipped.
 *
 * Neither @test_description nor @directive shall contain newline characters.
 */
void gtu_log_test_skipped (const char* test_description, const char* directive);

/**
 * gtu_log_test_failed:
 * @text_description: (allow-none): description of the test.
 * @directive:        (allow-none): additional information about the skip.
 *
 * Indicate to the test harness that a test has failed.
 *
 * Neither @test_description nor @directive shall contain newline characters.
 */
void gtu_log_test_failed (const char* test_description, const char* directive);

#endif
