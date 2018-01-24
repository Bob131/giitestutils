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
