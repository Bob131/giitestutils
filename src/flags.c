#include "gtu-priv.h"

GtuTestModeFlags gtu_test_mode_flags_get_flags (void) {
  GtuTestModeFlags flags = 0;

  /* the flags are set at initialisation */
  g_return_val_if_fail (gtu_has_initialized (), 0);

  if (g_test_perf ())
    flags |= GTU_TEST_MODE_FLAGS_PERF;

  if (g_test_slow ())
    flags |= GTU_TEST_MODE_FLAGS_SLOW;

  if (g_test_quick ())
    flags |= GTU_TEST_MODE_FLAGS_QUICK;

  if (g_test_undefined ())
    flags |= GTU_TEST_MODE_FLAGS_UNDEFINED;

  if (g_test_verbose ())
    flags |= GTU_TEST_MODE_FLAGS_VERBOSE;

  if (g_test_quiet ())
    flags |= GTU_TEST_MODE_FLAGS_QUIET;

  /* sanity checking */
  {
    /* some flags must be set */
    g_assert (flags != 0);

    /* one of SLOW or QUICK must be set */
    g_assert (flags & GTU_TEST_MODE_FLAGS_SLOW ||
              flags & GTU_TEST_MODE_FLAGS_QUICK);
    /* SLOW and QUICK are mutually exclusive */
    g_assert ((flags & GTU_TEST_MODE_FLAGS_SLOW)  == 0 ||
              (flags & GTU_TEST_MODE_FLAGS_QUICK) == 0);

    /* VERBOSE and QUIET are mutually exclusive */
    g_assert ((flags & GTU_TEST_MODE_FLAGS_VERBOSE) == 0 ||
              (flags & GTU_TEST_MODE_FLAGS_QUIET)   == 0);
  }

  return flags;
}

bool _gtu_should_log (GLogLevelFlags log_level) {
  if (log_level & G_LOG_FLAG_FATAL)
    return true;

  switch (log_level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:
    case G_LOG_LEVEL_CRITICAL:
    case G_LOG_LEVEL_WARNING:
      return true;

    case G_LOG_LEVEL_MESSAGE:
      return !(gtu_test_mode_flags_get_flags () & GTU_TEST_MODE_FLAGS_QUIET);

    case G_LOG_LEVEL_INFO:
    case G_LOG_LEVEL_DEBUG:
      return gtu_test_mode_flags_get_flags () & GTU_TEST_MODE_FLAGS_VERBOSE;

    default:
      g_assert_not_reached ();
  }
}
