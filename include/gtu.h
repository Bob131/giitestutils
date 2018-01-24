#ifndef __GII_TEST_UTILS_H__
#define __GII_TEST_UTILS_H__

#include <stdbool.h>
#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define gtu_assert(cond) G_STMT_START { \
  if (G_UNLIKELY (!(cond)))             \
    gtu_assert_not_reached ();          \
} G_STMT_END

#define gtu_skip_if(cond, message) G_STMT_START { \
  if (G_UNLIKELY (cond))                          \
    gtu_skip_if_reached (message);                \
} G_STMT_END

#define gtu_skip_if_not_thorough() \
  gtu_skip_if (g_test_quick (), "Thorough tests disabled")

#define gtu_skip_if_not_perf() \
  gtu_skip_if (!g_test_perf (), "Performance tests disabled")

typedef void (*GtuTestFunc) (void* target);

void gtu_init (char** args, int args_length);
bool gtu_has_initialized (void);

void gtu_assert_not_reached (void)                G_GNUC_NORETURN;
void gtu_skip_if_reached    (const char* message) G_GNUC_NORETURN;

GTestCase* gtu_create_test_case (const char* name,
                                 GtuTestFunc func,
                                 void* func_target,
                                 GDestroyNotify func_target_destroy);

static inline int gtu_run_suite (GTestSuite* suite) {
  g_test_suite_add_suite (g_test_get_root (), suite);
  return g_test_run ();
}

G_END_DECLS

#endif
