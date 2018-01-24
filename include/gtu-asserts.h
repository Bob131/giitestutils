#ifndef __GII_TEST_UTILS_ASSERTS_H__
#define __GII_TEST_UTILS_ASSERTS_H__

#ifndef __GII_TEST_UTILS_H__
#error "Only <gtu.h> can be included directly."
#endif

G_BEGIN_DECLS

void gtu_skip_if_reached (const char* message) G_GNUC_NORETURN;

#define gtu_skip_if(condition, message) G_STMT_START { \
  if (G_UNLIKELY (condition))                          \
    gtu_skip_if_reached (message);                     \
} G_STMT_END

#define gtu_skip_if_not_thorough() \
  gtu_skip_if (g_test_quick (), "Thorough tests disabled")

#define gtu_skip_if_not_perf() \
  gtu_skip_if (!g_test_perf (), "Performance tests disabled")


void _gtu_assertion_message (const char* file,
                             const char* line,
                             const char* function,
                             const char* message) G_GNUC_NORETURN;

#define gtu_assert_with_message(condition, message) G_STMT_START {          \
  if (G_LIKELY (condition))                                                 \
    ;                                                                       \
  else                                                                      \
    _gtu_assertion_message (__FILE__, G_STRINGIFY (__LINE__), G_STRFUNC,    \
                            message);                                       \
} G_STMT_END

#define gtu_assert_not_reached() \
  gtu_assert_with_message (false, "code should not be reached")

#define gtu_assert(condition) \
  gtu_assert_with_message (condition, "assertion failed: " #condition)

#define gtu_assert_true(expression) \
  gtu_assert_with_message ((expression), "should be true: " #expression)

#define gtu_assert_false(expression) \
  gtu_assert_with_message (!(expression), "should be false: " #expression)

#define gtu_assert_null(expression) \
  gtu_assert_with_message ((expression) == NULL, "should be NULL: " #expression)

#define gtu_assert_nonnull(expression)              \
  gtu_assert_with_message ((expression) != NULL,    \
                           "shouldn't be NULL: " #expression)

G_END_DECLS

#endif
