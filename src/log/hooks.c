#include "priv.h"
#include "logio.h"
#include "log-hooks.h"

#define UNW_LOCAL_ONLY
#include <libunwind.h>


/*
  Hook handling is complicated by the need to be able to automatically back
  out of the execution of whatever coded logged a particular message. This is
  handled by GtuTestCase using setjmp(), but we can't do that here: GLib
  internally maintains some global state that it uses to detect messages which
  have been recursively logged. Without allowing GLib to clean up after our log
  handler returns, subsequent invocations of g_log() will be considered
  recursive and the program will abort.

  To clarify, the problem at hand is as follows: we need to return to g_logv
  from our log handler as normal, allowing the function to clean up, but
  somehow regain control once this is done so GtuTestCase can do its thing.

  The solution I landed on is somewhat absurd, so here's a list of things I
  investigated that sound less horrid but are ultimately not as robust:

    - Copying gmessages.c in-tree and overriding GLib functions using tricks
      with linker scripts or LD_PRELOAD: fragile and not terribly portable.

    - Copying gmessages.c in-tree and overriding GLib functions by fiddling
      with the PLT/GOT: not at all portable, *extremely* fragile, likely to
      be thwarted by future security mechanisms and doesn't do much for calls
      routed through the GOT compiled into the GLib shared object (which,
      AFAIU, can't be fiddled as they're part of the shared address space).

    - Re-declare the relevant objects as `extern' and do the clean up
      ourselves: presents a lot of opportunity for getting out of sync with
      upstream as internal mechanisms evolve, and allows us to make matters
      worse by treading on GLib's toes.

  The best, most robust (but still pretty fragile), most portable (or not)
  solution I could come up with is as follows:

    1. At start-up, make a note of where g_logv() lives.

    2. If our log handler makes the decision to abort, use libunwind to unwind
       the call stack and grab the stack frame belonging to g_logv().

    3. Take the stack frame above and clobber the UNW_REG_IP pseudo-register.
       This pseudo-register is how libunwind models stack return addresses, so
       whatever address we write here is where the CPU will jump when g_logv()
       returns.

    4. Restore g_logv()'s stack frame, resuming execution in that function as
       though we returned normally.
*/


typedef struct _Hook Hook;

struct _Hook {
  GtuLogHook  func;
  const void* target;
  Hook* next;
};

G_LOCK_DEFINE_STATIC (hooks);
static Hook* hooks = NULL;

/* we can't just take the address of g_logv() because of the PLT */
static void (*glogv_address) (const char*, int, const char*, va_list) = NULL;

static void (*abort_handler_address) (void) = NULL;

static char* _log_domain = NULL;


static void do_abort (void) {
  /* we don't want to log on assertion failure */
# define assert(x) G_STMT_START { if (x); else g_abort (); } G_STMT_END

  int unw_res;
  unw_context_t unwind_context;
  unw_cursor_t cursor;
  unw_cursor_t glogv_cursor;

  assert (unw_getcontext (&unwind_context) == 0);
  assert (unw_init_local (&cursor, &unwind_context) == 0);

  {
    unw_proc_info_t procedure_info;
    bool found_glogv = false;

    while ((unw_res = unw_step (&cursor)) > 0) {
      assert (unw_get_proc_info (&cursor, &procedure_info) == 0);

      if (procedure_info.start_ip == (unw_word_t) glogv_address) {
        found_glogv = true;
        break;
      }
    }

    /* assert that there wasn't an error and we didn't reach the last frame */
    assert (unw_res > 0);
    assert (found_glogv);
  }

  glogv_cursor = cursor;

  /* The return pseudo-register for g_logv() is on the stack frame above.
     Overwrite it with `abort_handler_address'. */
  assert (unw_step (&cursor) > 0);
  unw_res = unw_set_reg (&cursor,
                         UNW_REG_IP,
                         (unw_word_t) abort_handler_address);
  assert (unw_res == 0);

  /* jump up the stack into g_logv() so it can clean up */
  unw_resume (&glogv_cursor);
  g_abort (); /* assert not reached */

# undef assert
}

static bool invoke_hooks (const GtuLogGMessage* message, void* user_data) {
  Hook* cursor;

  (void) user_data;

  G_LOCK (hooks);

  for (cursor = hooks; cursor != NULL; cursor = cursor->next) {
    GtuLogAction action;

    g_assert (cursor->func != NULL);
    action = cursor->func (message, (void*) cursor->target);

    if (action == GTU_LOG_ACTION_CONTINUE)
      continue;

    G_UNLOCK (hooks);

    if (action == GTU_LOG_ACTION_IGNORE)
      return true;

    if (action == GTU_LOG_ACTION_SUPPRESS) {
      GString* string = g_string_new (NULL);

      GtuLogGMessage suppress_message = {
        _log_domain,
        G_LOG_LEVEL_INFO,
        "Suppressed message: "
      };

      gtu_log_g_format_message_append (string, &suppress_message);
      gtu_log_g_format_message_append (string, message);
      g_string_append_c (string, '\n');

      gtu_log_diagnostic (string->str);

      g_string_free (string, true);
      return true;
    }

    if (action == GTU_LOG_ACTION_ABORT)
      do_abort ();

    if (action == GTU_LOG_ACTION_BAIL_OUT)
      gtu_log_bail_out (false, gtu_log_g_format_message (message->domain,
                                                         message->flags,
                                                         message->body));

    g_assert_not_reached ();
  }

  G_UNLOCK (hooks);
  return false;
}

static void find_glogv_address (const char* domain,
                                GLogLevelFlags level,
                                const char* message,
                                void* user_data)
{
  unw_context_t unwind_context;
  unw_cursor_t cursor;
  unw_proc_info_t procedure_info;

  (void) domain;
  (void) level;
  (void) message;
  (void) user_data;

  g_assert (glogv_address == NULL);

  g_assert (unw_getcontext (&unwind_context) == 0);
  g_assert (unw_init_local (&cursor, &unwind_context) == 0);

  g_assert (unw_step (&cursor) > 0);
  g_assert (unw_get_proc_info (&cursor, &procedure_info) == 0);

  glogv_address =
    (void(*)(const char*,int,const char*,va_list)) procedure_info.start_ip;
}

void gtu_log_hooks_init (const char* log_domain, void (*abort_handler) (void)) {
  static volatile size_t has_initialized = 0;

  if (g_once_init_enter (&has_initialized)) {
    g_assert (_log_domain == NULL);
    _log_domain = g_strdup (log_domain);

    gtu_log_g_install_handlers ();
    gtu_log_g_register_internal_domain (log_domain);

    unsigned handler_id = g_log_set_handler (
      NULL, G_LOG_LEVEL_MESSAGE, &find_glogv_address, NULL
    );
    g_log (NULL, G_LOG_LEVEL_MESSAGE, "test");
    g_log_remove_handler (NULL, handler_id);

    g_assert (glogv_address != NULL);

    g_assert (abort_handler != NULL);
    abort_handler_address = abort_handler;

    gtu_log_g_install_suppress_func (&invoke_hooks, NULL);

    g_once_init_leave (&has_initialized, 1);
  }
}

void gtu_log_hooks_push (GtuLogHook hook, const void* user_data) {
  Hook* new_hook;

  g_assert (hook != NULL);
  G_LOCK (hooks);

  new_hook = g_slice_new (Hook);
  new_hook->func = hook;
  new_hook->target = user_data;
  new_hook->next = hooks;

  hooks = new_hook;
  G_UNLOCK (hooks);
}

void gtu_log_hooks_pop (GtuLogHook hook) {
  Hook *cursor,
       *cursor_prev = NULL;

  g_assert (hooks != NULL);
  G_LOCK (hooks);

  for (cursor = hooks;
       cursor != NULL;
       cursor_prev = cursor, cursor = cursor->next)
  {
    if (cursor->func != hook)
      continue;

    if (cursor_prev != NULL)
      cursor_prev->next = cursor->next;
    else
      hooks = cursor->next;

    g_slice_free (Hook, cursor);

    G_UNLOCK (hooks);
    return;
  }

  g_assert_not_reached ();
}
