#include <string.h>
#include "priv.h"
#include "log/log-glib.h"
#include "log/log-color.h"

#define UNW_LOCAL_ONLY
#include <libunwind.h>

static bool suppress_callback (const char* domain,
                               GLogLevelFlags level,
                               const char* message,
                               uintptr_t caller,
                               void* user_data)
{
  unsigned i;
  GtuTestCasePrivate* priv;
  GtuTestSuite* parent_suite;
  GtuTestCase* self;

  g_assert (GTU_IS_TEST_CASE (user_data));
  self = GTU_TEST_CASE (user_data);

  g_assert (message != NULL);

  if (domain == NULL || strcmp (domain, GTU_LOG_DOMAIN) == 0)
    return false;

  priv = PRIVATE (self);

  for (i = 0; i < priv->expected_msgs->len; i++) {
    GtuExpectedMessage* expect =
      &g_array_index (priv->expected_msgs, GtuExpectedMessage, i);

    if (strcmp (expect->domain, domain) != 0)
      continue;

    if ((level & expect->flags) == 0)
      continue;

    if (g_regex_match (expect->regex, message, G_REGEX_MATCH_NOTEMPTY, NULL)) {
      g_atomic_int_inc (&expect->match_count.s);
      return true;
    }
  }

  parent_suite = gtu_test_object_get_parent_suite (GTU_TEST_OBJECT (self));
  if (_gtu_test_suite_log_should_fail (parent_suite, domain, level)) {

    /* We can't just pre-empt the test, as our logging machinery has been
       called by GLib's which maintains some global state regarding recursive
       messages; if we jump up the stack without returning, GLib never gets to
       record that the log handler finished.

       To remedy this, we do something delightfully evil:
         1. We unwind the stack until we've found the frame belonging to
            `caller'.
         2. `g_logv' is the stack frame above, so iterate the cursor again.
         3. The return address for `g_logv' is on the frame above, so iterate
            just one more time.
         4. Overwrite the return address with the address for
            `_gtu_test_preempt'.
         5. Jump up the stack to `g_logv' so it can clean up and return. */

    /* we don't want to log on assertion failure */
#   define assert(x) G_STMT_START { if (x); else g_abort (); } G_STMT_END

    GString* fail_message;

    int unw_res;
    unw_context_t unwind_context;
    unw_cursor_t cursor;
    unw_cursor_t glog_cursor;
    unw_proc_info_t procedure_info;
    bool found_caller = false;
    TestRunContext* tr_context;

    fail_message = g_string_new ("Unexpected fatal message: ");
    gtu_log_g_format_message_append (fail_message, domain, level, message);

    assert (unw_getcontext (&unwind_context) == 0);
    assert (unw_init_local (&cursor, &unwind_context) == 0);

    while ((unw_res = unw_step (&cursor)) > 0) {
      assert (unw_get_proc_info (&cursor, &procedure_info) == 0);

      if (procedure_info.start_ip == caller) {
        found_caller = true;
        break;
      }
    }

    /* assert that there wasn't an error and we didn't reach the last frame */
    assert (unw_res > 0);
    assert (found_caller);

    assert (unw_step (&cursor) > 0);
    glog_cursor = cursor;

    assert (unw_step (&cursor) > 0);
    unw_res = unw_set_reg (&cursor,
                           UNW_REG_IP,
                           (unw_word_t) &_gtu_test_preempt);

    assert (unw_res == 0);

    tr_context = _gtu_get_tr_context ();
    tr_context->message = g_string_free (fail_message, false);
    tr_context->result = GTU_TEST_RESULT_FAIL;

    unw_resume (&glog_cursor);
    assert (false);

#   undef assert
  }

  return false;
}

GtuTestResult _gtu_test_case_run (GtuTestCase* self, char** out_message) {
  char* message = NULL;
  const char* path;
  GtuTestCasePrivate* priv;

  g_assert (GTU_IS_TEST_CASE (self));

  priv = PRIVATE (self);
  path = gtu_test_object_get_path_string (GTU_TEST_OBJECT (self));

  if (priv->result == GTU_TEST_RESULT_INVALID) {
    gtu_log_g_install_suppress_func (&suppress_callback, self);

    g_info ("%s>>> %s%s",
            gtu_log_lookup_color (GTU_LOG_COLOR_FLAG_BOLD),
            path,
            gtu_log_lookup_color (GTU_LOG_COLOR_DISABLE));

    if (GTU_IS_COMPLEX_CASE (self)) {
      priv->result = _gtu_complex_case_run (GTU_COMPLEX_CASE (self), &message);
    } else {
      priv->result =
        _gtu_test_case_exec_inner (priv->func, priv->func_target, &message);
    }

    g_info ("%s<<< %s%s",
            gtu_log_lookup_color (GTU_LOG_COLOR_FLAG_BOLD),
            path,
            gtu_log_lookup_color (GTU_LOG_COLOR_DISABLE));

    gtu_log_g_uninstall_suppress_func (&suppress_callback);

    _gtu_test_case_dispose (self);
  }

  if (out_message)
    *out_message = message;

  return priv->result;
}
