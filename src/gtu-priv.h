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

G_GNUC_INTERNAL GtuTestObject* _gtu_test_object_construct (GType type);

G_GNUC_INTERNAL GTestCase* _gtu_test_case_get_g_case (GtuTestCase*);

G_GNUC_INTERNAL GTestCase*
_gtu_create_g_test_case (const char* name,
                         GtuTestCaseFunc func,
                         void* func_target,
                         GDestroyNotify func_target_destroy);

#endif
