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

/* defined in test-suite.c for access to test suite private data */
G_GNUC_INTERNAL void _gtu_test_object_collect_tests (GtuTestObject* object,
                                                     GPtrArray* tests);

G_GNUC_INTERNAL void _gtu_test_object_emit_ancestry_signal (GtuTestObject* obj);

typedef enum {
  GTU_TEST_RESULT_INVALID,
  GTU_TEST_RESULT_PASS,
  GTU_TEST_RESULT_SKIP,
  GTU_TEST_RESULT_FAIL
} GtuTestResult;

/* message may be NULL */
G_GNUC_INTERNAL GtuTestResult _gtu_test_case_run (GtuTestCase* self,
                                                  char** message);

G_GNUC_INTERNAL bool _gtu_test_case_has_run (GtuTestCase* self);

/* returns array of GtuTestObject */
G_GNUC_INTERNAL GPtrArray* _gtu_test_case_get_deps (GtuTestCase* self);

typedef struct {
  GList* path_selectors;
  GList* path_skippers;
  bool list_only;
} GtuTestMode;

G_GNUC_INTERNAL GtuTestMode* _gtu_get_test_mode (void);

G_GNUC_INTERNAL bool _gtu_path_element_is_valid (const char* element);

#endif
