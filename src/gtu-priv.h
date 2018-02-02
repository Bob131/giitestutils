#ifndef __GII_TEST_UTILS_PRIV_H__
#define __GII_TEST_UTILS_PRIV_H__

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#undef G_DISABLE_ASSERT
#include "gtu.h"

typedef enum {
  GTU_DEBUG_FLAGS_NONE = 0,
  GTU_DEBUG_FLAGS_FATAL_ASSERTS = 1 << 0
} GtuDebugFlags;

G_GNUC_INTERNAL GtuDebugFlags _gtu_debug_flags_get (void);

G_GNUC_INTERNAL void _gtu_test_object_set_parent_suite (GtuTestObject* self,
                                                        GtuTestSuite* parent);

G_GNUC_INTERNAL GtuTestObject* _gtu_test_object_construct (GType type,
                                                           const char* name);

typedef enum {
  GTU_TEST_RESULT_INVALID,
  GTU_TEST_RESULT_PASS,
  GTU_TEST_RESULT_SKIP,
  GTU_TEST_RESULT_FAIL
} GtuTestResult;

G_GNUC_INTERNAL GtuTestResult _gtu_test_case_run (GtuTestCase* self,
                                                  char** message);

#endif
