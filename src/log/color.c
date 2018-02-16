#define _POSIX_C_SOURCE 201111L
#include <stdio.h>
#include <stdint.h>

#include "priv.h"
#include "log-color.h"

#if !STRUCTURED_LOGGING_AVAILABLE
# include <unistd.h>
#endif

/* we probably don't want to be calling isatty() all the time for something
   trivial, so we wrap it in a once_init */
bool gtu_log_supports_color (void) {
  /* -1 for unsupported, +1 for supported */
  static volatile gssize supported = 0;

  if (g_once_init_enter (&supported)) {
    bool is_tty;
    gssize result;

#if STRUCTURED_LOGGING_AVAILABLE
    is_tty = g_log_writer_supports_color (fileno (stdout));
#else
    is_tty = isatty (fileno (stdout));
#endif

    result = is_tty ? +1 : -1;
    g_once_init_leave (&supported, result);
  }

  return supported > 0 ? true : false;
}

const char* gtu_log_lookup_color (GtuLogColor color) {
  if (!gtu_log_supports_color ())
    return "";

  switch ((uint16_t) color) {
    case GTU_LOG_COLOR_DISABLE: return "\033[0m";

#   include "color-table.c.in"

    default:
      g_assert_not_reached ();
  }
}
