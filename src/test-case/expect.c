#include <string.h>
#include "priv.h"

void _gtu_expected_message_dispose (GtuExpectedMessage* expected) {
  if (expected->domain) {
    g_free (expected->domain);
    expected->domain = NULL;
  }

  if (expected->regex) {
    g_regex_unref (expected->regex);
    expected->regex = NULL;
  }

  expected->match_count.s = 0;
  expected->flags = 0;
}

GtuExpectHandle gtu_test_case_expect_message (GtuTestCase* self,
                                              const char* domain,
                                              GLogLevelFlags level,
                                              GRegex* regex)
{
  GtuTestCasePrivate* priv;
  GtuExpectHandle ret;
  GtuExpectedMessage* msg;

  g_return_val_if_fail (GTU_IS_TEST_CASE (self),              -1);
  g_return_val_if_fail (domain != NULL && domain[0] != '\0',  -1);
  g_return_val_if_fail (strcmp (GTU_LOG_DOMAIN, domain) != 0, -1);

  priv = PRIVATE (self);
  ret = priv->expected_msgs->len;

  g_array_set_size (priv->expected_msgs, ret + 1);
  msg = &g_array_index (priv->expected_msgs, GtuExpectedMessage, ret);

  msg->domain = g_strdup (domain);
  msg->regex = regex;
  msg->flags = level;

  return ret;
}

bool gtu_test_case_expect_check (GtuTestCase* self, GtuExpectHandle handle) {
  GtuTestCasePrivate* priv;
  GtuExpectedMessage* msg;

  g_return_val_if_fail (GTU_IS_TEST_CASE (self), false);
  priv = PRIVATE (self);

  g_return_val_if_fail (handle < priv->expected_msgs->len, false);
  msg = &g_array_index (priv->expected_msgs, GtuExpectedMessage, handle);

  return msg->match_count.u > 0;
}

unsigned gtu_test_case_expect_count (GtuTestCase* self,
                                     GtuExpectHandle handle)
{
  GtuTestCasePrivate* priv;
  GtuExpectedMessage* msg;

  g_return_val_if_fail (GTU_IS_TEST_CASE (self), false);
  priv = PRIVATE (self);

  g_return_val_if_fail (handle < priv->expected_msgs->len, false);
  msg = &g_array_index (priv->expected_msgs, GtuExpectedMessage, handle);

  return g_atomic_int_and (&msg->match_count.u, 0);
}
