#ifndef __GII_TEST_UTILS_TEST_CASE_PRIV_H__
#define __GII_TEST_UTILS_TEST_CASE_PRIV_H__

#include "gtu-priv.h"
#include "priv-setjmp.h"

typedef struct {
  GtuTestCaseFunc   func;
  void*             func_target;
  GDestroyNotify    func_target_destroy;
  GArray*           expected_msgs; /* array of ExpectedMessage */
  GtuTestResult     result;
  bool              has_disposed;  /* FALSE if we're valid, TRUE if we've been
                                      executed and subsequently freed all
                                      internally held resources. */
} GtuTestCasePrivate;

#define PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTU_TYPE_TEST_CASE, GtuTestCasePrivate))

G_GNUC_INTERNAL void _gtu_test_case_dispose (GtuTestCase* self);

typedef struct {
  char*          domain;
  GRegex*        regex;

  union {
    volatile int s;
    volatile unsigned u;
  } match_count;

  GLogLevelFlags flags;
} GtuExpectedMessage;

G_GNUC_INTERNAL void
_gtu_expected_message_dispose (GtuExpectedMessage* message);

#endif
