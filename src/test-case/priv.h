#ifndef __GII_TEST_UTILS_TEST_CASE_PRIV_H__
#define __GII_TEST_UTILS_TEST_CASE_PRIV_H__

#include "gtu-priv.h"

G_GNUC_INTERNAL GtuTestResult _gtu_test_case_exec_inner (GtuTestCaseFunc func,
                                                         void* func_target,
                                                         char** message);

#endif
