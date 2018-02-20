#include <string.h>
#include "gtu-priv.h"

G_DEFINE_INTERFACE (GtuLogConsumer, gtu_log_consumer, GTU_TYPE_TEST_OBJECT)

typedef struct {
  char* domain;
  GLogLevelFlags flags;
} FatalMessage;

struct __GtuLogConsumerPrivate { GArray array; };
static GArray* PRIVATE (void* self) {
  return &GTU_LOG_CONSUMER_GET_IFACE (self)->
    get_private (GTU_LOG_CONSUMER (self))->array;
}

static void fatal_message_dispose (FatalMessage* message) {
  if (message->domain) {
    g_free (message->domain);
    message->domain = NULL;
  }

  message->flags = 0;
}

void gtu_log_consumer_fail_if_logged (void* self,
                                      const char* domain,
                                      GLogLevelFlags level)
{
  GArray* priv;
  unsigned index;
  FatalMessage* message;

  g_return_if_fail (GTU_IS_LOG_CONSUMER (self));
  g_return_if_fail (domain != NULL);
  g_return_if_fail (domain[0] != '\0' &&
                    !g_str_has_prefix (domain, GTU_LOG_DOMAIN));

  if (level == 0)
    level = G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING;

  priv = PRIVATE (self);
  index = priv->len;
  g_array_set_size (priv, index + 1);
  message = &g_array_index (priv, FatalMessage, index);

  message->domain = g_strdup (domain);
  message->flags = level;
}

bool _gtu_log_consumer_should_fail (void* self_ptr,
                                    const char* domain,
                                    GLogLevelFlags flags)
{
  GArray* priv;
  unsigned i;

  g_assert (domain != NULL);

  if (self_ptr == NULL || !GTU_IS_LOG_CONSUMER (self_ptr))
    return false;

  priv = PRIVATE (self_ptr);

  for (i = 0; i < priv->len; i++) {
    FatalMessage* message = &g_array_index (priv, FatalMessage, i);

    if (strcmp (message->domain, domain) != 0)
      continue;

    if ((flags & message->flags) == 0)
      continue;

    return true;
  }

  return _gtu_log_consumer_should_fail (
    gtu_test_object_get_parent_suite (GTU_TEST_OBJECT (self_ptr)),
    domain,
    flags
  );
}

_GtuLogConsumerPrivate* _gtu_log_consumer_private_new (void) {
  GArray* ret = g_array_new (false, true, sizeof (FatalMessage));
  g_array_set_clear_func (ret, (GDestroyNotify) &fatal_message_dispose);
  return (_GtuLogConsumerPrivate*) ret;
}

void _gtu_log_consumer_private_free (_GtuLogConsumerPrivate* priv) {
  g_array_free (&priv->array, true);
}

static void gtu_log_consumer_default_init (GtuLogConsumerInterface* klass) {
  (void) klass;
}
