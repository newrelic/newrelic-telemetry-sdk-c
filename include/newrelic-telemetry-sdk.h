#ifndef NEWRELIC_TELEMETRY_SDK
#define NEWRELIC_TELEMETRY_SDK

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _nrt_sender_t nrt_sender_t;

/**
 * Create a new sender.
 *
 * Senders can be used to send span and metrics batches.
 *
 * @param key an Insights API key
 * @return a sender
 */
nrt_sender_t *nrt_sender_new(const char *key);

/**
 * Shutdown a sender.
 *
 * Shuts down the sender, sends pending data and frees the sender object.
 *
 * @param sender a sender
 */
void nrt_sender_shutdown(nrt_sender_t *sender);

#ifdef __cplusplus
}
#endif

#endif /* NEWRELIC_TELEMETRY_SDK */
