#ifndef __GII_TEST_UTILS_SKIPS_H__
#define __GII_TEST_UTILS_SKIPS_H__

/**
 * SECTION:gtu-skips
 * @short_description: pre-test checking
 * @title: Skips
 * @include: gtu.h
 *
 * Test cases aren't always required (or able) to run on every suite invocation.
 * As such, GTU has facilities for performing pre-test checks. As
 * with <xref linkend="gtu-Asserts" endterm="gtu-Asserts.top_of_page"/>,
 * test execution is automatically aborted if a skip check fails.
 *
 * Please note that the result of calling any of these functions outside of a
 * GTU test case is undefined.
 */

#ifndef __GII_TEST_UTILS_H__
#error "Only <gtu.h> can be included directly."
#endif

G_BEGIN_DECLS

/**
 * gtu_skip_if_reached:
 * @message: (allow-none): explanation of skip.
 *
 * Skips and aborts the current test, with an optional message describing why
 * the test was skipped.
 *
 * This function doesn't return to the caller, instead proceeding onto the next
 * test.
 */
void gtu_skip_if_reached (const char* message) G_GNUC_NORETURN;

/**
 * gtu_skip_if:
 * @condition: %FALSE for a no-op, %TRUE if this test should be skipped.
 * @message:   (allow-none): explanation of skip.
 *
 * If @condition is truthy, this test is skipped and we abort execution.
 *
 * See gtu_skip_if_reached().
 */
#define gtu_skip_if(condition, message) G_STMT_START { \
  if (G_UNLIKELY (condition))                          \
    gtu_skip_if_reached (message);                     \
} G_STMT_END

/**
 * gtu_skip_if_flags_unset:
 * @flags:   OR'd combination of #GtuTestModeFlags to test.
 * @message: (allow-none): explanation of skip if test fails.
 *
 * Tests the result of gtu_test_mode_flags_get_flags() against @flags. If any
 * flags are unset, this test is marked as skipped and aborted with an optional
 * message.
 */
#define gtu_skip_if_flags_unset(flags, message)                         \
  gtu_skip_if ((gtu_test_mode_flags_get_flags () & (flags)) != (flags), \
               message)

/**
 * gtu_skip_if_not_thorough:
 *
 * Skip and abort the current test if thorough tests have been disabled, e.g.
 * with the `--quick` command line flag.
 *
 * See #GtuTestModeFlags.
 */
#define gtu_skip_if_not_thorough()                       \
  gtu_skip_if_flags_unset (GTU_TEST_MODE_FLAGS_THOROUGH, \
                           "Thorough tests disabled")

/**
 * gtu_skip_if_not_perf:
 *
 * Skip and abort the current test if performance tests are disabled.
 *
 * See #GtuTestModeFlags.
 */
#define gtu_skip_if_not_perf()                       \
  gtu_skip_if_flags_unset (GTU_TEST_MODE_FLAGS_PERF, \
                           "Performance tests disabled")

/**
 * gtu_skip_if_not_undefined:
 *
 * Skip and abort the current test if tests which invoke undefined behaviour
 * have been disabled.
 *
 * See #GtuTestModeFlags.
 */
#define gtu_skip_if_not_undefined()                       \
  gtu_skip_if_flags_unset (GTU_TEST_MODE_FLAGS_UNDEFINED, \
                           "Tests invoking undefined behaviour are disabled")

G_END_DECLS

#endif
