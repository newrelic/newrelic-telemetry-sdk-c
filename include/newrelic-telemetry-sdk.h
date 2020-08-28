#ifndef NEWRELIC_TELEMETRY_SDK
#define NEWRELIC_TELEMETRY_SDK

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A Client is capable of both queuing and sending span and metrics batches to
 * a configured New Relic collector.
 */
typedef struct _nrt_client_t nrt_client_t;

/**
 * A span batch
 *
 * A collection of spans.
 */
typedef struct _nrt_span_batch_t nrt_span_batch_t;

/**
 * A span
 */
typedef struct _nrt_span_t nrt_span_t;

/**
 * A collection of attributes
 */
typedef struct _nrt_attributes_t nrt_attributes_t;

/**
 * Indicates a point in time or a duration.
 *
 * A point in time is indicated by a time in Epoch milliseconds, a duration is
 * indicated by milliseconds.
 */
typedef uint64_t nrt_time_t;

/**
 * Create a new attribute collection.
 *
 * Allocate and initialize an empty attribute collection
 *
 * @return an empty attribute collection
 */
nrt_attributes_t* nrt_attributes_new();

/**
 * Add an int attribute to an attribute collection
 *
 * @param attributes
 * @param key the attribute key
 * @param value the attribute value
 * @return true if the attribute was added
 */
bool nrt_attributes_set_int(nrt_attributes_t* attributes,
                            const char* key,
                            int64_t value);

/**
 * Add an unsigned int attribute to an attribute collection
 *
 * @param attributes
 * @param key the attribute key
 * @param value the attribute value
 * @return true if the attribute was added
 */
bool nrt_attributes_set_uint(nrt_attributes_t* attributes,
                             const char* key,
                             uint64_t value);

/**
 * Add a double attribute to an attribute collection
 *
 * @param attributes
 * @param key the attribute key
 * @param value the attribute value
 * @return true if the attribute was added
 */
bool nrt_attributes_set_double(nrt_attributes_t* attributes,
                               const char* key,
                               double value);

/**
 * Add a string attribute to an attribute collection
 *
 * @param attributes
 * @param key the attribute key
 * @param value the attribute value
 * @return true if the attribute was added
 */
bool nrt_attributes_set_string(nrt_attributes_t* attributes,
                               const char* key,
                               const char* value);

/**
 * Add a bool attribute to an attribute collection
 *
 * @param attributes
 * @param key the attribute key
 * @param value the attribute value
 * @return true if the attribute was added
 */
bool nrt_attributes_set_bool(nrt_attributes_t* attributes,
                             const char* key,
                             bool value);

/**
 * Destroy an attribute collection
 *
 * Destroy the attribute collection without adding it to a span. The passed
 * pointer will be set to NULL.
 *
 * @param attributes an attribute collection
 */
void nrt_attributes_destroy(nrt_attributes_t** attributes);

/**
 * Create a new span.
 *
 * Allocate and initialize a new span.
 *
 * @param id the span id
 * @param trace_id the trace id
 * @param parent_id the parent id. Passing NULL indicates that the span has no
 * parent.
 * @return a span
 */
nrt_span_t* nrt_span_new(const char* id,
                         const char* trace_id,
                         const char* parent_id);

/**
 * Set the name of a span
 *
 * @param span
 * @param name the name for the span
 * @return true if the name could be set
 */
bool nrt_span_set_name(nrt_span_t* span, const char* name);

/**
 * Set the service name of a span
 *
 * @param span
 * @param service_name the service name for the span
 * @return true if the service name could be set
 */
bool nrt_span_set_service_name(nrt_span_t* span, const char* service_name);

/**
 * Set the start timestamp for a span
 *
 * @param span
 * @param timestamp the start timestamp for the span in Epoch milliseconds
 * @return true if the start timestamp could be set
 */
bool nrt_span_set_timestamp(nrt_span_t* span, nrt_time_t timestamp);

/**
 * Set the duration for a span
 *
 * @param span
 * @param duration the duration for the span in milliseconds
 * @return true if the duration could be set
 */
bool nrt_span_set_duration(nrt_span_t* span, nrt_time_t duration);

/**
 * Set attributes on a span
 *
 * If the attributes were successfully added to the span, the span takes
 * ownership of the attribute collection. Otherwise the attribute collection
 * will be destroyed. The passed pointer to the attribute collection will
 * always be set to NULL.
 *
 * @param span
 * @param attributes a collection of attributes
 */
bool nrt_span_set_attributes(nrt_span_t* span, nrt_attributes_t** attributes);

/**
 * Destroy a span.
 *
 * Destroy a span without adding it to a batch. The passed pointer will be set
 * to NULL.
 *
 * @param span
 */
void nrt_span_destroy(nrt_span_t** span);

/**
 * Create a new span batch.
 *
 * A span batch is a collection of spans that is sent in one payload.
 *
 * @return an empty span batch
 */
nrt_span_batch_t* nrt_span_batch_new();

/**
 * Add a span to a span batch.
 *
 * If the span was successfully added to the batch, the batch takes ownership
 * of the span. Otherwise the span will be destroyed. The passed pointer will
 * always be set to NULL.
 *
 * @param batch a span batch
 * @param span a span
 * @return true if the span was successfully added to the batch
 */
bool nrt_span_batch_record(nrt_span_batch_t* batch, nrt_span_t** span);

/**
 * Destroy a span batch.
 *
 * Spans previously added to the span batch will be lost. The passed pointer
 * will be set to NULL.
 *
 * @param batch a span batch
 */
void nrt_span_batch_destroy(nrt_span_batch_t** batch);

/**
 * Create a new client.
 *
 * @param key an Insights API key
 * @return a client
 */
nrt_client_t* nrt_client_new(const char* key);

/**
 * Send a span batch.
 *
 * Put a span batch in the queue of the client. This function returns as soon
 * as the span batch was queued and doesn't wait for it to be sent
 * successfully.
 *
 * If the span batch is successfully queued, the client takes ownership of the
 * span batch. Otherwise the batch will be destroyed. The passed pointer will
 * always be set to NULL.
 *
 * @param client a client
 * @param client the span batch to be sent
 * @return true if the span batch was successfully queued
 */
bool nrt_client_send(nrt_client_t* client, nrt_span_batch_t** batch);

/**
 * Shutdown a client.
 *
 * Shuts down the client, sends pending data and frees the client object. The
 * passed pointer will be set to NULL.
 *
 * @param client a client
 */
void nrt_client_shutdown(nrt_client_t** client);

/**
 * Destroy a client.
 *
 * Destroy the client, without making sure pending data is sent. The passed
 * pointer will be set to NULL.
 *
 * @param client a client
 */
void nrt_client_destroy(nrt_client_t** client);

#ifdef __cplusplus
}
#endif

#endif /* NEWRELIC_TELEMETRY_SDK */
