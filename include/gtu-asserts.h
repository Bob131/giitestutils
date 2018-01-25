#ifndef __GII_TEST_UTILS_ASSERTS_H__
#define __GII_TEST_UTILS_ASSERTS_H__

/**
 * SECTION:gtu-asserts
 * @short_description: aborting tests early
 * @title: Asserts
 * @include: gtu.h
 *
 * GTU provides flow control management to unit tests, automatically backing
 * out of test execution when a test is aborted. To facilitate this mechanism,
 * we provide some drop-in replacements for GLib's assertion functions that can
 * be used within your unit tests.
 *
 * Please note that the result of calling any of these functions outside of a
 * GTU test case is undefined.
 */

#ifndef __GII_TEST_UTILS_H__
#error "Only <gtu.h> can be included directly."
#endif

G_BEGIN_DECLS

/**
 * _gtu_assertion_message: (skip)
 * @file:       #__FILE__
 * @line:       #__LINE__
 * @function:   #G_STRFUNC
 * @message:    (allow-none): assertion failure explanation.
 *
 * Print an error message and abort the current test.
 *
 * Do not call this function.
 */
void _gtu_assertion_message (const char* file,
                             const char* line,
                             const char* function,
                             const char* message) G_GNUC_NORETURN;

/**
 * gtu_assert_with_message:
 * @condition: %TRUE for a no-op, %FALSE if this test has failed and we should
 *             abort the test.
 * @message:   (allow-none): explanation of failure if @condition is %FALSE
 *
 * If @condition is truthy, do nothing. When @condition is falsy, we abort the
 * test and print @message with some extra diagnostic information.
 */
#define gtu_assert_with_message(condition, message) G_STMT_START {          \
  if (G_LIKELY (condition))                                                 \
    ;                                                                       \
  else                                                                      \
    _gtu_assertion_message (__FILE__, G_STRINGIFY (__LINE__), G_STRFUNC,    \
                            message);                                       \
} G_STMT_END

/**
 * gtu_assert_not_reached:
 *
 * Marks the current test as failed. Does not return.
 */
#define gtu_assert_not_reached() \
  gtu_assert_with_message (false, "code should not be reached")

/**
 * gtu_assert:
 * @condition: %TRUE for a no-op, %FALSE if this test has failed and we should
 *             abort the test.
 *
 * If @condition is truthy, do nothing. When @condition is falsy, we abort the
 * current test.
 */
#define gtu_assert(condition) \
  gtu_assert_with_message (condition, "assertion failed: " #condition)

/**
 * gtu_assert_true:
 * @expression: expression to be evaluated.
 *
 * See gtu_assert().
 */
#define gtu_assert_true(expression) \
  gtu_assert_with_message ((expression), "should be true: " #expression)

/**
 * gtu_assert_false:
 * @expression: expression to be evaluated.
 *
 * Performs the inverse of gtu_assert_true().
 */
#define gtu_assert_false(expression) \
  gtu_assert_with_message (!(expression), "should be false: " #expression)

/**
 * gtu_assert_null:
 * @expression: expression to be tested.
 *
 * Aborts the current test if @expression is not equal to %NULL.
 */
#define gtu_assert_null(expression) \
  gtu_assert_with_message ((expression) == NULL, "should be NULL: " #expression)

/**
 * gtu_assert_nonnull:
 * @expression: expression to be tested.
 *
 * Aborts the current test if @expression is equal to %NULL.
 */
#define gtu_assert_nonnull(expression)              \
  gtu_assert_with_message ((expression) != NULL,    \
                           "shouldn't be NULL: " #expression)

G_END_DECLS

#endif
