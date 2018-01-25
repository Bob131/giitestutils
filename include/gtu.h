#ifndef __GII_TEST_UTILS_H__
#define __GII_TEST_UTILS_H__

/**
 * SECTION:gtu
 * @short_description: utilities for the GLib test framework
 * @title: Gii Test Utilities
 * @include: gtu.h
 *
 * The Gii Test Utilities (or GTU) library provides some wrapper utilities atop
 * the GLib testing framework.
 */

#include <stdbool.h>
#include <glib.h>
#include <glib-object.h>

#include "gtu-asserts.h"
#include "gtu-skips.h"

G_BEGIN_DECLS

/**
 * gtu_init:
 * @args:        (array length=args_length): an array of arguments passed to
 *                                           main().
 * @args_length: the number of elements in @args.
 *
 * Initialises the GLib test framework and the GTU library.
 *
 * This function may be called several times.
 *
 * This function must be called before any other GTU functionality is used.
 */
void gtu_init (char** args, int args_length);

/**
 * gtu_has_initialized:
 *
 * Returns %TRUE if GTU has been initialised.
 *
 * Returns: %TRUE if gtu_init() has been called successfully, %FALSE otherwise.
 */
bool gtu_has_initialized (void);

/**
 * GtuTestModeFlags:
 * @GTU_TEST_MODE_FLAGS_PERF:       Enable performance tests.
 * @GTU_TEST_MODE_FLAGS_SLOW:       Enable slow tests and repeat
 *                                  non-deterministic tests more often.
 *                                  Mutually exclusive with
 *                                  @GTU_TEST_MODE_FLAGS_QUICK.
 * @GTU_TEST_MODE_FLAGS_QUICK:      Disable slow tests and do fewer iterations
 *                                  of non-deterministic tests.
 *                                  Mutually exclusive with
 *                                  @GTU_TEST_MODE_FLAGS_SLOW.
 * @GTU_TEST_MODE_FLAGS_UNDEFINED:  Enable tests that provoke assertion
 *                                  failures.
 * @GTU_TEST_MODE_FLAGS_VERBOSE:    Enable verbose logging and disable
 *                                  @GTU_TEST_MODE_FLAGS_QUIET.
 * @GTU_TEST_MODE_FLAGS_QUIET:      Silence normal output and disable
 *                                  @GTU_TEST_MODE_FLAGS_VERBOSE.
 * @GTU_TEST_MODE_FLAGS_THOROUGH:   Alias of @GTU_TEST_MODE_FLAGS_SLOW.
 *
 * Represents the options a user running the test suite has requested. See
 * %TESTMODE for more information.
 *
 * Notes on valid flag combinations:
 *  * The result of gtu_test_mode_flags_get_flags() should never be `0`; at
 *    least one flag will always be set.
 *
 *  * @GTU_TEST_MODE_FLAGS_SLOW/@GTU_TEST_MODE_FLAGS_THOROUGH should never
 *    be set with @GTU_TEST_MODE_FLAGS_QUICK.
 *
 *  * One of @GTU_TEST_MODE_FLAGS_SLOW or @GTU_TEST_MODE_FLAGS_QUICK will
 *    always be set in values returned from gtu_test_mode_flags_get_flags().
 *
 *  * @GTU_TEST_MODE_FLAGS_VERBOSE should never be set with
 *    @GTU_TEST_MODE_FLAGS_QUIET.
 *
 *  * It's valid for neither @GTU_TEST_MODE_FLAGS_VERBOSE or
 *    @GTU_TEST_MODE_FLAGS_QUIET to be set; this is the default.
 */
typedef enum {
  GTU_TEST_MODE_FLAGS_PERF         = 1 << 0,
  GTU_TEST_MODE_FLAGS_SLOW         = 1 << 1,
  GTU_TEST_MODE_FLAGS_QUICK        = 1 << 2,
  GTU_TEST_MODE_FLAGS_UNDEFINED    = 1 << 3,
  GTU_TEST_MODE_FLAGS_VERBOSE      = 1 << 4,
  GTU_TEST_MODE_FLAGS_QUIET        = 1 << 5,

  GTU_TEST_MODE_FLAGS_THOROUGH = GTU_TEST_MODE_FLAGS_SLOW
} GtuTestModeFlags;

/**
 * gtu_test_mode_flags_get_flags:
 *
 * Returns the #GtuTestModeFlags that the current test suite has been launched
 * with.
 *
 * Returns: flags for the current test suite.
 */
GtuTestModeFlags gtu_test_mode_flags_get_flags (void);

/**
 * GtuTestFunc:
 * @target: (closure): pointer to user data.
 *
 * A user-supplied function within which a test case is implemented.
 */
typedef void (*GtuTestFunc) (void* target);

/**
 * gtu_create_test_case:
 * @name:                the name of the new test case.
 * @func:                function implementing a test.
 * @func_target:         (allow-none) (transfer full) (closure):
 *                       pointer to user data, passed to @func.
 * @func_target_destroy: (allow-none) (destroy): frees @func_target.
 *
 * Creates a new test case from which GTU flow control facilities (e.g.
 * gtu_skip_if(), gtu_assert()) can be used.
 *
 * Returns: a new #GTestCase.
 */
GTestCase* gtu_create_test_case (const char* name,
                                 GtuTestFunc func,
                                 void* func_target,
                                 GDestroyNotify func_target_destroy);

#ifndef __GTK_DOC_IGNORE__
static inline int gtu_run_suite (GTestSuite* suite) {
  int ret;
  static bool has_run = false;
  g_return_val_if_fail (!has_run, 1);

  g_test_suite_add_suite (g_test_get_root (), suite);
  ret = g_test_run ();
  has_run = true;
  return ret;
}
#endif

/* hack around gtk-doc */
#define __NOT_GTK_DOC__
#ifndef __NOT_GTK_DOC__

/**
 * gtu_run_suite:
 * @suite: a GLib test suite to be executed.
 *
 * Convenience function that adds @suite to the global #GTestSuite and calls
 * g_test_run().
 *
 * No further tests may be run (since g_test_run() may only be called once), so
 * any tests that should be run must be added to @suite before calling this
 * function.
 *
 * Returns: result from g_test_run().
 */
int gtu_run_suite (GTestSuite* suite);

#endif
#undef __NOT_GTK_DOC__

G_END_DECLS

#endif
