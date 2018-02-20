#ifndef __GII_TEST_UTILS_LOG_CONSUMER_PRIV_H__
#define __GII_TEST_UTILS_LOG_CONSUMER_PRIV_H__

#include "gtu-priv.h"

G_GNUC_INTERNAL _GtuLogConsumerPrivate* _gtu_log_consumer_private_new (void);

G_GNUC_INTERNAL void
_gtu_log_consumer_private_free (_GtuLogConsumerPrivate* priv);

/* `self' needn't be a GtuLogConsumer or even a valid pointer, as we check for
   this ourselves.

   Domain mustn't be NULL. */
G_GNUC_INTERNAL bool _gtu_log_consumer_should_fail (void* self,
                                                    const char* domain,
                                                    GLogLevelFlags flags);

#endif
