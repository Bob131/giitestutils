#ifndef __GII_TEST_UTILS_H__
#define __GII_TEST_UTILS_H__

/**
 * SECTION:gtu
 * @short_description: global library state
 * @title: Initialisation
 * @include: gtu.h
 *
 * The library must be initialised before GTU features can be used. This
 * involves passing the arguments from your test suite binary's `main()` to
 * gtu_init(), which then sets up GTU's internal state ready for testing.
 *
 * Test flags parsed from command line arguments can be queried with
 * gtu_test_mode_flags_get_flags().
 */

/**
 * GTU_LOG_DOMAIN:
 *
 * GLib log domain for the GTU library. This shouldn't have a use for projects
 * using GTU, it's exposed as "good practice".
 *
 * See g_log().
 */
#define GTU_LOG_DOMAIN ("GiiTestUtils")

#include <stdbool.h>
#include <glib.h>
#include <glib-object.h>

#include "gtu-path.h"
#include "gtu-object.h"
#include "gtu-case.h"
#include "gtu-complex.h"
#include "gtu-suite.h"

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
 * It is safe to call this function more than once; further calls are a no-op
 * and ignore any passed arguments.
 *
 * This function must be called before any other GTU functionality is used.
 */
void gtu_init (char** args, int args_length);

/**
 * gtu_has_initialized:
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
 * [Test binaries][gtu-Args] for more information.
 *
 * Notes on valid flag combinations:
 *  * The result of gtu_test_mode_flags_get_flags() will never be `0`; at least
 *    one flag will always be set.
 *
 *  * @GTU_TEST_MODE_FLAGS_SLOW/@GTU_TEST_MODE_FLAGS_THOROUGH will never
 *    be set simultaneously with @GTU_TEST_MODE_FLAGS_QUICK.
 *
 *  * One of @GTU_TEST_MODE_FLAGS_SLOW or @GTU_TEST_MODE_FLAGS_QUICK will
 *    always be set in values returned from gtu_test_mode_flags_get_flags().
 *
 *  * @GTU_TEST_MODE_FLAGS_VERBOSE will never be set simultaneously with
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
 * Returns the #GtuTestModeFlags parsed from the command line arguments passed
 * to gtu_init().
 *
 * Returns: flags with which the current test program has been launched.
 */
GtuTestModeFlags gtu_test_mode_flags_get_flags (void);

G_END_DECLS

#endif
