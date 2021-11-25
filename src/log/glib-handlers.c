#include <stdlib.h>
#include <string.h>

#include "priv.h"
#include "logio.h"
#include "log-glib.h"

G_LOCK_DEFINE_STATIC (suppress_info);
static struct {
  GtuLogGSuppressFunc func;
  const void* data;
} suppress_info = {NULL, NULL};

/* re-parse GLib debug flags we're interested in */
static const GDebugKey glib_debug_keys[] = {
  { "fatal-criticals", G_LOG_LEVEL_CRITICAL },
  { "fatal-warnings",  G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL }
};

static GLogLevelFlags fatal_mask;

#define SHOULD_SUPPRESS(domain, level, message) \
  (should_suppress ((domain), &(level), (message)))

static bool should_suppress (const char* domain,
                             GLogLevelFlags* level,
                             const char* message)
{
  bool ret;
  GtuLogGMessage g_message = { domain, *level, message };

  /* TODO: hold read locks. The suppression mechanism is exclusively for the
     benefit of GtuTestCase, which does all sorts of nasty stack unwinding
     trickery. If we naively lock the mutex, GtuTestCase will destroy our
     internal state. */

  /* G_LOCK (suppress_info); */

  ret = suppress_info.func != NULL ?
    suppress_info.func (&g_message, (void*) suppress_info.data) :
    false;

  /* G_UNLOCK (suppress_info); */

  *level = g_message.flags;
  return ret;
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
  gtu_log_bail_out (true, formatted_message);
}

static void message_handler (const char* domain,
                             GLogLevelFlags level,
                             const char* message,
                             void* data)
{
  (void) data;

  if (!SHOULD_SUPPRESS (domain, level, message)) {
    if (level & G_LOG_FLAG_FATAL) {
      message_bailout (domain, level, message);
    } else {
      message_printer (domain, level, message);
    }
  }
}

#if STRUCTURED_LOGGING_AVAILABLE
static GLogWriterOutput structured_handler (GLogLevelFlags level,
                                            const GLogField* fields,
                                            size_t n_fields,
                                            void* data)
{
  size_t i;
  const char* domain = NULL;
  const char* message = NULL;
  GString* formatted_message;

  (void) data;

  /* the always-fatal mask doesn't has no effect on structured logs, so we set
   * this ourselves */
  if (level & fatal_mask)
    level |= G_LOG_FLAG_FATAL;

  /* collect the domain/message fields and check suppression */
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
  if (message != NULL && SHOULD_SUPPRESS (domain, level, message))
    goto ret;

  formatted_message = g_string_new ("Structured message:");

  if (message != NULL) {
    GtuLogGMessage g_message = { domain, level, message };

    g_string_append_c (formatted_message, ' ');
    gtu_log_g_format_message_append (formatted_message, &g_message);
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

  if (level & G_LOG_FLAG_FATAL)
    gtu_log_bail_out (true, "Fatal structured message received");

ret:
  return G_LOG_WRITER_HANDLED;
}
#endif

static void log_empty_plan (void) {
  gtu_log_test_plan (0);
}

/* GTU log handlers are responsible for decisions about aborting programs, but
 * for fatal messages GLib preempts them by aborting early. This handler is
 * just to get GLib to pass fatal messages on. */
static gboolean never_abort (const char* log_domain,
                             GLogLevelFlags log_level,
                             const char* message,
                             void* user_data)
{
  (void) log_domain;
  (void) log_level;
  (void) message;
  (void) user_data;
  return false;
}

void gtu_log_g_install_handlers (bool fatal_warnings) {
  static size_t has_installed;

  if (g_once_init_enter (&has_installed)) {
    g_set_print_handler (&stdfd_handler);
    g_set_printerr_handler (&stdfd_handler);

    fatal_mask = g_parse_debug_string (
      getenv ("G_DEBUG"),
      glib_debug_keys,
      G_N_ELEMENTS (glib_debug_keys));
    if (fatal_warnings)
      fatal_mask |= (G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);
    g_log_set_always_fatal (fatal_mask);

    g_log_set_default_handler (&message_handler, NULL);
#if STRUCTURED_LOGGING_AVAILABLE
    g_log_set_writer_func (&structured_handler, NULL, NULL);
#endif

    g_test_log_set_fatal_handler (&never_abort, NULL);

    /* If we exit without running any tests, this handler will log an empty
     * test plan; otherwise, gtu_log_test_plan will return and the handler is
     * effectively a no-op */
    atexit (&log_empty_plan);

    gtu_log_g_register_internal_domain (G_LOG_DOMAIN);

    g_once_init_leave (&has_installed, 1);
  }
}

void gtu_log_g_register_internal_domain (const char* domain) {
  g_assert (domain != NULL);
  g_log_set_fatal_mask (domain, G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);
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
