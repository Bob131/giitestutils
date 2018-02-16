#ifndef __GII_TEST_UTILS_LOG_PRIV_H__
#define __GII_TEST_UTILS_LOG_PRIV_H__

#undef G_DISABLE_CHECKS
#undef G_DISABLE_ASSERT

#include <glib.h>

#define STRUCTURED_LOGGING_AVAILABLE \
    (GLIB_VERSION_MAX_ALLOWED >= GLIB_VERSION_2_50)

#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN ("GiiTestUtils-Log")

#endif
