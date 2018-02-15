#ifndef __GII_TEST_UTILS_LOG_GLIB_H__
#define __GII_TEST_UTILS_LOG_GLIB_H__

#include <stdbool.h>
#include <stdint.h>
#include <glib.h>

/**
 * GtuLogGSuppressFunc:
 * @domain:        (allow-none): GLib log domain.
 * @level:         GLib log level/flags.
 * @message:       a message logged with @domain and @level.
 * @gtu_log_entry: the address of the first GTU Log function called by GLib.
 * @user_data:     (closure): user-provided data.
 *
 * Callback to determine whether a message should be suppressed. Must not call
 * gtu_log_g_*_suppress_func.
 *
 * Returns: %TRUE if @message should be suppressed, %FALSE otherwise.
 */
typedef bool (*GtuLogGSuppressFunc) (const char* domain,
                                     GLogLevelFlags level,
                                     const char* message,
                                     uintptr_t gtu_log_entry,
                                     void* user_data);

/**
 * gtu_log_g_install_handlers:
 *
 * On the first call, installs a variety of callbacks in GLib to ensure message
 * logging is handled correctly. On subsequent calls, does nothing.
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
 * @domain:  (allow-none): log domain to which @message belongs.
 * @level:   logging level of @message, including flags.
 * @message: contents of the logged message.
 *
 * Appends a formatted string describing a message logged via GLib to @string.
 */
void gtu_log_g_format_message_append (GString* string,
                                      const char* domain,
                                      GLogLevelFlags level,
                                      const char* message);

/**
 * gtu_log_g_format_message:
 * @domain:  (allow-none): log domain to which @message belongs.
 * @level:   logging level of @message with flags.
 * @message: contents of the logged message.
 *
 * Returns a string describing a message logged via GLib.
 *
 * Returns: an owned string, without a trailing newline. Free the result when
 *          done.
 */
char* gtu_log_g_format_message (const char* domain,
                                GLogLevelFlags level,
                                const char* message);

#endif
