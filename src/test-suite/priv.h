#ifndef __GII_TEST_UTILS_TEST_SUITE_PRIV_H__
#define __GII_TEST_UTILS_TEST_SUITE_PRIV_H__

#include "gtu-priv.h"

/* exit status to signal an error to the automake harness */
#define TEST_ERROR ((int) 99)

G_GNUC_INTERNAL int _gtu_test_suite_run_internal (GPtrArray* tests);

#endif
