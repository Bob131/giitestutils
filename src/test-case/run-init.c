#include <string.h>
#include "priv.h"
#include "log/log-color.h"
#include "log/log-hooks.h"

static GtuLogAction log_hook (GtuLogGMessage* message, void* user_data) {
  GtuTestCase* self;

  g_assert (GTU_IS_TEST_CASE (user_data));
  self = GTU_TEST_CASE (user_data);

  g_assert (message != NULL);

  /* Note: we assume the user does not/cannot add an expectation for a GTU
   *       logging domain, so any such check is omitted here. */
  if (message->domain != NULL) {
    unsigned i;
    GtuTestCasePrivate* priv = PRIVATE (self);

    for (i = 0; i < priv->expected_msgs->len; i++) {
      GtuExpectedMessage* expect =
        &g_array_index (priv->expected_msgs, GtuExpectedMessage, i);

      if (strcmp (expect->domain, message->domain) != 0)
        continue;

      if ((message->flags & expect->flags) == 0)
        continue;

      if (g_regex_match (expect->regex, message->body,
                         G_REGEX_MATCH_NOTEMPTY,
                         NULL))
      {
        g_atomic_int_inc (&expect->match_count.s);
        return _gtu_should_log (G_LOG_LEVEL_INFO) ?
          GTU_LOG_ACTION_SUPPRESS :
          GTU_LOG_ACTION_IGNORE;
      }
    }
  }

  if (message->flags & G_LOG_FLAG_FATAL)
    return GTU_LOG_ACTION_CONTINUE;

  if (message->flags & (G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING)) {
    GString* fail_message;
    TestRunContext* tr_context;

    fail_message = g_string_new ("Unexpected message: ");
    gtu_log_g_format_message_append (fail_message, message);

    tr_context = _gtu_get_tr_context ();
    tr_context->message = g_string_free (fail_message, false);
    tr_context->result = GTU_TEST_RESULT_FAIL;

    return GTU_LOG_ACTION_ABORT;
  }

  return GTU_LOG_ACTION_CONTINUE;
}

GtuTestResult _gtu_test_case_run (GtuTestCase* self, char** out_message) {
  char* message = NULL;
  const char* path;
  GtuTestCasePrivate* priv;

  g_assert (GTU_IS_TEST_CASE (self));

  priv = PRIVATE (self);
  path = gtu_test_object_get_path_string (GTU_TEST_OBJECT (self));

  if (priv->result == GTU_TEST_RESULT_INVALID) {
    gtu_log_hooks_push (&log_hook, self);

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

    gtu_log_hooks_pop (&log_hook);

    _gtu_test_case_dispose (self);
  }

  if (out_message)
    *out_message = message;

  return priv->result;
}
