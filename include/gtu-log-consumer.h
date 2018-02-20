#ifndef __GII_TEST_UTILS_LOG_CONSUMER_H__
#define __GII_TEST_UTILS_LOG_CONSUMER_H__

/**
 * SECTION:gtu-log-consumer
 * @short_description: log-driven tests
 * @title: Log Consumers
 * @include: gtu.h
 *
 * #GtuLogConsumer is an interface implemented by test objects whose state can
 * be influenced by messages emitted via GLib's logging system.
 *
 * The interface exists for the purpose of providing a common public API for
 * GTU's test objects; the interface itself is a private implementation detail,
 * and should not be derived.
 */

#ifndef __GII_TEST_UTILS_H__
#error "Only <gtu.h> can be included directly."
#endif

G_BEGIN_DECLS

/**
 * GTU_TYPE_LOG_CONSUMER:
 *
 * #GType for #GtuLogConsumer objects.
 */
#define GTU_TYPE_LOG_CONSUMER (gtu_log_consumer_get_type ())
G_DECLARE_INTERFACE (GtuLogConsumer, gtu_log_consumer,
                     GTU, LOG_CONSUMER,
                     GtuTestObject)

#ifndef __GTK_DOC_IGNORE__
typedef struct __GtuLogConsumerPrivate _GtuLogConsumerPrivate;
#endif

/**
 * GtuLogConsumer:
 *
 * A #GtuTestObject that can be configured to change state based on messages
 * passed to g_log().
 */

/**
 * GtuLogConsumerInterface:
 *
 * Interface implemented by supporting #GtuTestObject types. All fields are
 * private to GTU.
 */
struct _GtuLogConsumerInterface {
  /*< private >*/
  GTypeInterface parent_iface;
  _GtuLogConsumerPrivate* (*get_private) (GtuLogConsumer* self);
};

/**
 * gtu_log_consumer_fail_if_logged:
 * @self:   (type Gtu.LogConsumer): log consumer instance.
 * @domain: log domain to check against.
 * @level:  log flags to check against.
 *
 * Indicates that, should the execution of @self result in the logging of any
 * messages matching @domain and @level, the current test will have failed.
 *
 * @domain mustn't be %NULL or zero-length. The result is undefined if @domain
 * is equal to %GTU_LOG_DOMAIN.
 *
 * For a logged message to match @level, the message's log level flags and
 * @level must have some common bits set. If @level is zero, it defaults to
 * <literal>%G_LOG_LEVEL_CRITICAL | %G_LOG_LEVEL_WARNING</literal>.
 *
 * Matchers configured with this function have a higher precedence than GTU's
 * generic bail-out handler, and a lower precedence than those configured with
 * gtu_test_case_expect_message().
 */
void gtu_log_consumer_fail_if_logged (void* self,
                                      const char* domain,
                                      GLogLevelFlags level);

G_END_DECLS

#endif
