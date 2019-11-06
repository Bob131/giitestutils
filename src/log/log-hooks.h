#ifndef __GII_TEST_UTILS_LOG_HOOKS_H__
#define __GII_TEST_UTILS_LOG_HOOKS_H__

#include "log-glib.h"

/**
 * GtuLogAction:
 *
 * Return value to indicate how a hook wants the message handled.
 */
typedef enum {

  /**
   * GTU_LOG_ACTION_CONTINUE:
   *
   * Do nothing and run the next hook.
   */
  GTU_LOG_ACTION_CONTINUE,

  /**
   * GTU_LOG_ACTION_IGNORE:
   *
   * Do nothing and stop processing hooks.
   */
  GTU_LOG_ACTION_IGNORE,

  /**
   * GTU_LOG_ACTION_SUPPRESS:
   *
   * Emit suppression message and stop processing hooks.
   */
  GTU_LOG_ACTION_SUPPRESS,

  /**
   * GTU_LOG_ACTION_ABORT:
   *
   * Abort the test in-progress by mangling call stack return values and other
   * really horrible stuff.
   *
   * THIS IS REALLY DANGEROUS!
   */
  GTU_LOG_ACTION_ABORT,

  /**
   * GTU_LOG_ACTION_BAIL_OUT:
   *
   * Signals to the test harness that a fatal error has occured and exits the
   * program.
   */
  GTU_LOG_ACTION_BAIL_OUT

} GtuLogAction;

/**
 * GtuLogHook:
 * @message:   message logged via GLib.
 * @user_data: (closure): user-provided data.
 *
 * Callback executed whenever a GLib message is logged.
 *
 * Returns: how the hook wants the message to be handled.
 */
typedef GtuLogAction (*GtuLogHook) (const GtuLogGMessage* message,
                                    void* user_data);

/**
 * gtu_log_hooks_init:
 * @log_domain:    log domain used when formatting suppression messages and
 *                 passed to gtu_log_g_install_handlers().
 * @abort_handler: address to write on the call stack on hook abortion.
 *
 * Initialises internal library state and registers @abort_handler as the abort
 * handler. When a hook returns %GTU_LOG_ACTION_ABORT, @abort_handler will
 * clobber return addresses in certain stack frames on the call stack, with the
 * intended effect that the abort handler will be called after log handlers
 * have cleaned up.
 *
 * It's important that abort handlers do not do excessive stack allocations
 * (e.g. by calling sprintf) as the stack will be in an inconsistent state and
 * this may cause segfaults. The only thing it should do is use longjmp (or a
 * similar mechanism) to jump up to a sane point in the stack.
 *
 * For log hooks to function correctly, gtu_log_g_install_handlers() mustn't be
 * used; this is called automatically. gtu_log_g_install_suppress_func() (and
 * its counterpart) also mustn't be used.
 *
 * This function must be called *after* g_test_init(). See
 * gtu_log_g_install_handlers() for details.
 *
 * On subsequent calls, this function is a no-op.
 */
void gtu_log_hooks_init (const char* log_domain, void (*abort_handler) (void));

/**
 * gtu_log_hooks_push:
 * @hook:       hook to install.
 * @user_data:  user-provided data passed to @hook on execution.
 *
 * Push @hook to the top of the hook stack. The hook will be executed before
 * any others already on the stack, hence can be considered as being of a
 * higher precedence.
 */
void gtu_log_hooks_push (GtuLogHook hook, const void* user_data);

/**
 * gtu_log_hooks_pop:
 * @hook: hook to pop off the hook stack.
 *
 * Removes @hook from the hook stack, preventing it from being called upon
 * receiving further messages. It is an error to call this function if @hook
 * does not exist on the stack.
 */
void gtu_log_hooks_pop (GtuLogHook hook);

#endif
