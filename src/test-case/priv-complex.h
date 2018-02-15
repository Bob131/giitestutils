#ifndef __GII_TEST_UTILS_COMPLEX_CASE_PRIV_H__
#define __GII_TEST_UTILS_COMPLEX_CASE_PRIV_H__

#include "gtu-priv.h"

/* message may not be NULL */
G_GNUC_INTERNAL GtuTestResult _gtu_complex_case_run (GtuComplexCase* self,
                                                     char** message);

#endif
