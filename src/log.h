#ifndef __GII_TEST_UTILS_TEST_LOG_PRIV_H__
#define __GII_TEST_UTILS_TEST_LOG_PRIV_H__

#include "gtu-priv.h"

G_GNUC_INTERNAL void _gtu_install_glib_loggers (void);

/* log raw message without any additional formatting beyond what's needed to
   make it safe for the test harness */
G_GNUC_INTERNAL void _gtu_log_printf (const char* format, ...);

/* if `n_tests' is zero, logs an empty test plan marked 'skip' */
G_GNUC_INTERNAL void _gtu_log_test_plan (unsigned n_tests);

/* `message' may be NULL */
G_GNUC_INTERNAL void _gtu_log_test_result (GtuTestResult result,
                                           const char* path,
                                           const char* message);

/* No trailing newline. Free the result when done */
G_GNUC_INTERNAL char* _gtu_log_format_message (const char* domain,
                                               GLogLevelFlags level,
                                               const char* message);

#endif
