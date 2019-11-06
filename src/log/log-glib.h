#ifndef __GII_TEST_UTILS_LOG_GLIB_H__
#define __GII_TEST_UTILS_LOG_GLIB_H__

#include <stdbool.h>
#include <stdint.h>
#include <glib.h>

/**
 * GtuLogGMessage:
 * @domain: (allow-none): GLib log domain.
 * @flags:  GLib log level + flags.
 * @body:   A message logged through the GLib logging machinery from @domain
 *          and with @flags.
 *
 * Contains all the details of a message logged with g_log() et al.
 */
typedef struct {
  const char* domain;
  GLogLevelFlags flags;
  const char* body;
} GtuLogGMessage;

/**
 * GtuLogGSuppressFunc:
 * @message:   a message logged via GLib.
 * @user_data: (closure): user-provided data.
 *
 * Callback to determine whether a message should be suppressed. Must not call
 * gtu_log_g_*_suppress_func.
 *
 * Returns: %TRUE if @message should be suppressed, %FALSE otherwise.
 */
typedef bool (*GtuLogGSuppressFunc) (const GtuLogGMessage* message,
                                     void* user_data);

/**
 * gtu_log_g_install_handlers:
 *
 * On the first call, installs a variety of callbacks in GLib to ensure message
 * logging is handled correctly. On subsequent calls, does nothing.
 *
 * This function parses the G_DEBUG environment variable and sets the
 * always-fatal mask based on the presence or absence of fatal-criticals and
 * fatal-warnings. As such, this function must be called after any other
 * functions that may alter the mask (i.e., g_test_init).
 */
void gtu_log_g_install_handlers (void);

/**
 * gtu_log_g_install_suppress_func:
 * @func:      suppress function.
 * @user_data: (closure) (allow-none): user data for @func.
 *
 * Install the callback @func, used to determine whether messages should be
 * suppressed.
 *
 * Should be matched with a call to gtu_log_g_uninstall_suppress_func().
 */
void gtu_log_g_install_suppress_func (GtuLogGSuppressFunc func,
                                      const void* user_data);

/**
 * gtu_log_g_uninstall_suppress_func:
 * @func: function to uninstall.
 *
 * Uninstalls @func. It is an error to call this function if @func had not been
 * previously installed.
 */
void gtu_log_g_uninstall_suppress_func (GtuLogGSuppressFunc func);

/**
 * @string:  #GString to which the formatted message will be appended.
 * @message: message to be formatted.
 *
 * Appends a formatted string describing a message logged via GLib to @string.
 */
void gtu_log_g_format_message_append (GString* string,
                                      const GtuLogGMessage* message);

/**
 * gtu_log_g_format_message:
 * @domain:  (allow-none): log domain to which @message belongs.
 * @level:   logging level of @message with flags.
 * @message: contents of the logged message.
 *
 * Convenience wrapper around gtu_log_g_format_message_append(), with a
 * signature more convenient for use from GLib handlers.
 *
 * Returns: an owned string, without a trailing newline. Free the result when
 *          done.
 */
char* gtu_log_g_format_message (const char* domain,
                                GLogLevelFlags level,
                                const char* message);

#endif
