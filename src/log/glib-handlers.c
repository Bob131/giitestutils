#include <stdlib.h>
#include <string.h>

#include "priv.h"
#include "logio.h"
#include "log-glib.h"

#include "gtu.h"

G_LOCK_DEFINE_STATIC (suppress_info);
static struct {
  GtuLogGSuppressFunc func;
  const void* data;
} suppress_info = {NULL, NULL};

#define SHOULD_SUPPRESS(domain, level, message, func) \
  (should_suppress ((domain), (level), (message), (uintptr_t) (func)))

static bool should_suppress (const char* domain,
                             GLogLevelFlags level,
                             const char* message,
                             uintptr_t log_entry)
{
  bool ret;

  /* TODO: hold read locks. The suppression mechanism is exclusively for the
     benefit of GtuTestCase, which does all sorts of nasty stack unwinding
     trickery. If we naively lock the mutex, GtuTestCase will destroy our
     internal state. */

  /* G_LOCK (suppress_info); */

  ret = suppress_info.func != NULL ?
    suppress_info.func (domain,
                        level,
                        message,
                        log_entry,
                        (void*) suppress_info.data) :
    false;

  /* G_UNLOCK (suppress_info); */

  return ret;
}

static bool should_log (GLogLevelFlags log_level) {
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

static void stdfd_handler (const char* message) {
  gtu_log_diagnostic (message);
}

static void message_printer (const char* domain,
                             GLogLevelFlags level,
                             const char* message)
{
  char* formatted_message = gtu_log_g_format_message (domain, level, message);
  gtu_log_diagnostic (formatted_message);
  g_free (formatted_message);
}

static void message_bailout (const char* domain,
                             GLogLevelFlags level,
                             const char* message)
{
  char* formatted_message = gtu_log_g_format_message (domain, level, message);
  gtu_log_bail_out (formatted_message);
}

static void message_handler (const char* domain,
                             GLogLevelFlags level,
                             const char* message,
                             void* data)
{
  (void) data;

  if (SHOULD_SUPPRESS (domain, level, message, &message_handler)) {
    GString* formatted_message;

    if (!should_log (G_LOG_LEVEL_INFO))
      return;

    formatted_message = g_string_new ("Suppressed message: ");
    gtu_log_g_format_message_append (formatted_message,
                                     domain,
                                     level,
                                     message);

    message_printer (GTU_LOG_DOMAIN, G_LOG_LEVEL_INFO, formatted_message->str);

    g_string_free (formatted_message, true);

  } else {
    if (!should_log (level))
      return;

    if (level & G_LOG_FLAG_FATAL)
      message_bailout (domain, level, message);
    else
      message_printer (domain, level, message);
  }
}

static GLogWriterOutput structured_handler (GLogLevelFlags level,
                                            const GLogField* fields,
                                            size_t n_fields,
                                            void* data)
{
  size_t i;
  const char* domain = NULL;
  const char* message = NULL;
  GString* formatted_message;
  bool suppressed = false;

  (void) data;

  {
    /* collect the domain/message fields and check suppression */
    GLogLevelFlags check_level = level;

    for (i = 0; i < n_fields && (message == NULL || domain == NULL); i++) {
      const char** var;

      if (strcmp (fields[i].key, "MESSAGE") == 0) {
        var = &message;
      } else if (strcmp (fields[i].key, "GLIB_DOMAIN") == 0) {
        var = &domain;
      } else {
        continue;
      }

      g_return_val_if_fail (fields[i].length == -1, G_LOG_WRITER_UNHANDLED);
      *var = (const char*) fields[i].value;
    }

    /* call SHOULD_SUPPRESS before we allocate any memory */
    if (message != NULL &&
        SHOULD_SUPPRESS (domain, level, message, &structured_handler))
    {
      check_level = G_LOG_LEVEL_INFO;
      suppressed = true;
    }

    if (!should_log (check_level))
      goto ret;
  }

  formatted_message = g_string_new (!suppressed ?
                                    "Structured message:" :
                                    "Structured message (suppressed):");

  if (message != NULL) {
    g_string_append_c (formatted_message, ' ');
    gtu_log_g_format_message_append (formatted_message,
                                     domain, level, message);

    if (suppressed) {
      char* info_message = gtu_log_g_format_message (GTU_LOG_DOMAIN,
                                                     G_LOG_LEVEL_INFO,
                                                     formatted_message->str);
      g_string_assign (formatted_message, info_message);
      g_free (info_message);
    }
  }

  g_string_append_c (formatted_message, '\n');

  for (i = 0; i < n_fields; i++) {
    char* value = NULL;

    if (fields[i].length == -1)
      value = g_strescape ((const char*) fields[i].value, "\t'\"");

    g_string_append_printf (formatted_message,
                            "'%s': %s\n",
                            fields[i].key,
                            value != NULL ? value : "<binary data>");

    if (value != NULL)
      g_free (value);
  }

  gtu_log_diagnostic (formatted_message->str);
  g_string_free (formatted_message, true);

  if (level & G_LOG_FLAG_FATAL && !suppressed)
    gtu_log_bail_out ("Fatal structured message received");

ret:
  return G_LOG_WRITER_HANDLED;
}

static gboolean fatal_handler (const char* domain,
                               GLogLevelFlags level,
                               const char* message,
                               void* data)
{
  (void) level;
  (void) message;
  (void) data;

  return domain == NULL ? false : g_str_has_prefix (domain, GTU_LOG_DOMAIN);
}

static void log_empty_plan (void) {
  gtu_log_test_plan (0);
}

void gtu_log_g_install_handlers (void) {
  static volatile size_t has_installed;

  if (g_once_init_enter (&has_installed)) {
    g_set_print_handler (&stdfd_handler);
    g_set_printerr_handler (&stdfd_handler);

    g_log_set_default_handler (&message_handler, NULL);
    g_log_set_writer_func (&structured_handler, NULL, NULL);

    g_test_log_set_fatal_handler (&fatal_handler, NULL);

    atexit (&log_empty_plan);

    g_once_init_leave (&has_installed, 1);
  }
}

void gtu_log_g_install_suppress_func (GtuLogGSuppressFunc func,
                                      const void* user_data)
{
  G_LOCK (suppress_info);

  if (suppress_info.func || suppress_info.data) {
    G_UNLOCK (suppress_info);
    g_return_if_reached ();
  }

  suppress_info.func = func;
  suppress_info.data = user_data;

  G_UNLOCK (suppress_info);
}

void gtu_log_g_uninstall_suppress_func (GtuLogGSuppressFunc func) {
  G_LOCK (suppress_info);

  if (suppress_info.func != func) {
    G_UNLOCK (suppress_info);
    g_return_if_reached ();
  }

  memset (&suppress_info, '\0', sizeof (suppress_info));

  G_UNLOCK (suppress_info);
}
