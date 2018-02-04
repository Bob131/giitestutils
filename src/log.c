#include <stdio.h>
#include "log.h"

/* TAP-compliant GLib message handling */

static bool should_log (GLogLevelFlags log_level) {
  if (log_level & G_LOG_FLAG_FATAL)
    return true;

  switch (log_level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:
    case G_LOG_LEVEL_CRITICAL:
    case G_LOG_LEVEL_WARNING:
    case G_LOG_LEVEL_MESSAGE:
      return true;

    case G_LOG_LEVEL_INFO:
    case G_LOG_LEVEL_DEBUG:
      return gtu_test_mode_flags_get_flags () & GTU_TEST_MODE_FLAGS_VERBOSE;

    default:
      g_assert_not_reached ();
  }
}

static const char* level_to_string (GLogLevelFlags log_level) {
#define ret(str) {                    \
  if (log_level & G_LOG_FLAG_FATAL) { \
    return "FATAL-" str;              \
  } else {                            \
    return str;                       \
  }                                   \
}

  switch (log_level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:
      ret ("ERROR");

    case G_LOG_LEVEL_CRITICAL:
      ret ("CRITICAL");

    case G_LOG_LEVEL_WARNING:
      ret ("WARNING");

    case G_LOG_LEVEL_MESSAGE:
      ret ("MESSAGE");

    case G_LOG_LEVEL_INFO:
      ret ("INFO");

    case G_LOG_LEVEL_DEBUG:
      ret ("DEBUG");

    default:
      g_assert_not_reached ();
  }

#undef ret
}

static void glib_test_logger (const char* log_domain, GLogLevelFlags log_level,
                              const char* message, void* data)
{
  (void) data;

  if (should_log (log_level))
    fprintf (stdout, "# %s%s%s: %s\n",
             log_domain != NULL ? log_domain : "",
             log_domain != NULL ? "-" : "",
             level_to_string (log_level),
             message);
}

static GLogWriterOutput glib_structured_logger (GLogLevelFlags log_level,
                                                const GLogField* fields,
                                                size_t n_fields,
                                                void* data)
{
  size_t i;

  (void) data;

  if (!should_log (log_level))
    goto ret;

  fprintf (stdout, "# %s: new structured message:\n",
           level_to_string (log_level));

  for (i = 0; i < n_fields; i++) {
    char* value = NULL;

    if (fields[i].length == -1)
      value = g_strescape ((const char*) fields[i].value, "\t'\"");

    fprintf (stdout, "#   '%s': %s\n",
             fields[i].key,
             value != NULL ? value : "<binary data>");

    if (value != NULL)
      g_free (value);
  }

ret:
  return G_LOG_WRITER_HANDLED;
}

void _gtu_install_glib_loggers (void) {
  g_log_set_default_handler (glib_test_logger, NULL);
  g_log_set_writer_func (glib_structured_logger, NULL, NULL);
}
