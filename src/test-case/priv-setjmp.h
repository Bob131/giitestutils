#ifndef __GII_TEST_UTILS_TEST_CASE_PRIV_SETJMP_H__
#define __GII_TEST_UTILS_TEST_CASE_PRIV_SETJMP_H__

#include <setjmp.h>
#include "gtu-priv.h"

typedef struct {
  uint32_t magic;
  jmp_buf caller_context;
  char* message;
  GtuTestResult result;
} TestRunContext;

G_GNUC_INTERNAL TestRunContext* _gtu_get_tr_context (void);

G_GNUC_INTERNAL bool _gtu_have_tr_context (void);

G_GNUC_INTERNAL GtuTestResult _gtu_test_case_exec_inner (GtuTestCaseFunc func,
                                                         void* func_target,
                                                         char** message);

G_GNUC_INTERNAL void _gtu_test_preempt ();

#endif
