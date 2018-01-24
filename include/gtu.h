#ifndef __GII_TEST_UTILS_H__
#define __GII_TEST_UTILS_H__

#include <stdbool.h>
#include <glib.h>
#include <glib-object.h>

#include "gtu-asserts.h"

G_BEGIN_DECLS

typedef void (*GtuTestFunc) (void* target);

void gtu_init (char** args, int args_length);

bool gtu_has_initialized (void);

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
