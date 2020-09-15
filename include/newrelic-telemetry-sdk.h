/**
 * @file newrelic-telemetry-sdk.h
 *
 * @brief This is a helper library that supports sending New Relic data
 * from within your C/C++ application.
 *
 * See accompanying README and LICENSE for more information.
 */
#ifndef NEWRELIC_TELEMETRY_SDK
#define NEWRELIC_TELEMETRY_SDK

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief A configuration object used to initialize a nrt_client_t.
 */
typedef struct _nrt_client_config_t nrt_client_config_t;

/**
 *  @brief A Client is capable of both queuing and sending span and metrics
 * batches to a configured New Relic collector.
 */
typedef struct _nrt_client_t nrt_client_t;

/**
 * @brief A span batch.
 *
 * This holds a collection of spans.
 * See the
 * [specification](https://github.com/newrelic/newrelic-telemetry-sdk-specs/blob/master/capabilities.md#span-batch)
 * for further details.
 */
typedef struct _nrt_span_batch_t nrt_span_batch_t;

/**
 * @brief A span.
 *
 * See the
 * [specification](https://github.com/newrelic/newrelic-telemetry-sdk-specs/blob/master/capabilities.md#spans)
 * for further details.
 */
typedef struct _nrt_span_t nrt_span_t;

/**
 * @brief A collection of attributes.
 *
 * Allows you to add custom attributes to spans.
 */
typedef struct _nrt_attributes_t nrt_attributes_t;

/**
 * @brief Indicates a point in time or a duration.
 *
 * A point in time is indicated by a time in Epoch milliseconds, a duration is
 * indicated by milliseconds.
 */
typedef uint64_t nrt_time_t;

/**
 * @brief Create a new attribute collection.
 *
 * THis allocates and initializes an empty attribute collection.
 *
 * @return An empty attribute collection.
 */
nrt_attributes_t* nrt_attributes_new();

/**
 * @brief Represents the available verbosity levels of the logger.
 */
typedef enum {
  NRT_LOG_ERROR = 0,
  NRT_LOG_WARN = 1,
  NRT_LOG_INFO = 2,
  NRT_LOG_DEBUG = 3,
  NRT_LOG_TRACE = 4,
} nrt_log_level_t;

/**
 * @brief Initialize logging.
 *
 * This will cause the Telemetry SDK to log messages of the given level to the
 * file of the given name.
 *
 * @param level The log level.
 * @param filename The name of the file to log messages to. For example: if you
 * want to log to stdout or stderr data streams, specify `stdout` or
 * `stderr` as the filename.
 *
 * @return True if log was initialized.
 */
bool nrt_log_init(nrt_log_level_t level, const char* filename);

/**
 * @brief Add an int attribute to an attribute collection.
 *
 * @param attributes An attribute collection.
 * @param key The attribute key.
 * @param value The attribute value.
 * @return True if the attribute was added.
 */
bool nrt_attributes_set_int(nrt_attributes_t* attributes,
                            const char* key,
                            int64_t value);

/**
 * @brief Add an unsigned int attribute to an attribute collection.
 *
 * @param attributes An attribute collection.
 * @param key The attribute key.
 * @param value The attribute value.
 * @return True if the attribute was added.
 */
bool nrt_attributes_set_uint(nrt_attributes_t* attributes,
                             const char* key,
                             uint64_t value);

/**
 * @brief Add a double attribute to an attribute collection.
 *
 * @param attributes An attribute collection.
 * @param key The attribute key.
 * @param value The attribute value.
 * @return True if the attribute was added.
 */
bool nrt_attributes_set_double(nrt_attributes_t* attributes,
                               const char* key,
                               double value);

/**
 * @brief Add a string attribute to an attribute collection.
 *
 * @param attributes An attribute collection.
 * @param key The attribute key.
 * @param value The attribute value.
 * @return True if the attribute was added.
 */
bool nrt_attributes_set_string(nrt_attributes_t* attributes,
                               const char* key,
                               const char* value);

/**
 * @brief Add a bool attribute to an attribute collection.
 *
 * @param attributes An attribute collection.
 * @param key The attribute key.
 * @param value The attribute value.
 * @return True if the attribute was added.
 */
bool nrt_attributes_set_bool(nrt_attributes_t* attributes,
                             const char* key,
                             bool value);

/**
 * @brief Destroy an attribute collection.
 *
 * This destroys the attribute collection without adding it to a span. The
 * passed pointer will be set to NULL.
 *
 * @param attributes An attribute collection.
 */
void nrt_attributes_destroy(nrt_attributes_t** attributes);

/**
 * @brief Create a new span.
 *
 * This Allocates and initialize a new span.
 *
 * @param id A span id.
 * @param trace_id The trace id.
 * @param timestamp The timestamp.
 * @return A span.
 */
nrt_span_t* nrt_span_new(const char* id,
                         const char* trace_id,
                         uint64_t timestamp);

/**
 * @brief Set the id of a span.
 *
 * @param span A span.
 * @param id The unique identifier for the span.
 * @return True if the id could be set.
 */
bool nrt_span_set_id(nrt_span_t* span, const char* id);

/**
 * @brief Set the trace_id of a span.
 *
 * @param   span A span.
 * @param   trace_id The trace_id for the span.
 * @return  True if the trace_id could be set.
 */
bool nrt_span_set_trace_id(nrt_span_t* span, const char* trace_id);

/**
 * @brief Set the start timestamp for a span.
 *
 * @param   span A span.
 * @param   timestamp The start timestamp for the span.
 * @return  True if the start timestamp could be set.
 */
bool nrt_span_set_timestamp(nrt_span_t* span, nrt_time_t timestamp);

/**
 * @brief Set the name of a span.
 *
 * @param span A span.
 * @param name The name for the span.
 * @return True if the name could be set.
 */
bool nrt_span_set_name(nrt_span_t* span, const char* name);

/**
 * @brief Set the service name of a span.
 *
 * @param span A span.
 * @param service_name The service name for the span.
 * @return True if the service name could be set.
 */
bool nrt_span_set_service_name(nrt_span_t* span, const char* service_name);

/**
 * @brief Set the parent_id of a span.
 *
 * @param span A span.
 * @param parent_id The parent_id for the span.
 * @return True if the parent_id could be set.
 */
bool nrt_span_set_parent_id(nrt_span_t* span, const char* parent_id);

/**
 * @brief Set the duration for a span.
 *
 * @param span A span.
 * @param duration The duration for the span in milliseconds.
 * @return True if the duration could be set.
 */
bool nrt_span_set_duration(nrt_span_t* span, nrt_time_t duration);

/**
 * @brief Set attributes on a span.
 *
 * If the attributes were successfully added to the span, the span takes
 * ownership of the attribute collection. Otherwise the attribute collection
 * will be destroyed. The passed pointer to the attribute collection will
 * always be set to NULL.
 *
 * @param span A span.
 * @param attributes An attribute collection.
 * @return true if the attributes were added.
 */
bool nrt_span_set_attributes(nrt_span_t* span, nrt_attributes_t** attributes);

/**
 * @brief Destroy a span.
 *
 * This destroys a span without adding it to a batch. The passed pointer will be
 * set to NULL.
 *
 * @param span A span.
 */
void nrt_span_destroy(nrt_span_t** span);

/**
 * @brief Create a new span batch.
 *
 * A span batch is a collection of spans that is sent in one payload. This
 * creates an empty span batch collection
 *
 * @return An empty span batch.
 */
nrt_span_batch_t* nrt_span_batch_new();

/**
 * @brief Add a span to a span batch.
 *
 * If the span was successfully added to the batch, the batch takes ownership
 * of the span. Otherwise the span will be destroyed. The passed pointer will
 * always be set to NULL.
 *
 * @param batch A span batch.
 * @param span A span.
 * @return True if the span was successfully added to the batch.
 */
bool nrt_span_batch_record(nrt_span_batch_t* batch, nrt_span_t** span);

/**
 * @brief Destroy a span batch.
 *
 * This destroys a span batch. Any spans previously added to the span batch will
 * be lost. The passed pointer will be set to NULL.
 *
 * @param batch A span batch.
 */
void nrt_span_batch_destroy(nrt_span_batch_t** batch);

/**
 * @brief Create a new client configuration with an Insights API key.
 *
 * Other values will be set to defaults:
 *  - The default backoff factor will be 5 seconds.
 *  - The default maximum of retries is 8.
 *  - The default trace endpoint is * https://trace-api.newrelic.com/trace/v1`
 *    on port 80.
 *  - By default, product information is empty.
 *  - By default, no more than 100 batches are sent in one go.
 *
 * @param key An Insights API key.
 * @return A client configuration.
 */
nrt_client_config_t* nrt_client_config_new(const char* key);

/**
 * @brief Configures a backoff factor.
 *
 * If a request fails, the SDK retries the request at increasing intervals
 * and eventually drops data if the request cannot be completed.
 *
 * The amount of time to wait after a request can be computed using this
 * logic:
 *
 *   `backoff_factor * (2 ^ (number_of_retries - 1))`
 *
 * For a backoff factor of 1 second, and a maximum of 6 retries,
 * the retry delay interval should follow a pattern of [0, 1, 2, 4, 8, 16].
 *
 * See the
 * [specification](https://github.com/newrelic/newrelic-telemetry-sdk-specs/blob/master/communication.md#graceful-degradation)
 * for further details.
 *
 * @param config A client configuration.
 * @param backoff_factor A time duration used in the backoff calculation.
 */
void nrt_client_config_set_backoff_factor(nrt_client_config_t* config,
                                          nrt_time_t backoff_factor);

/**
 * @brief Configures the maximum numbers of retries.
 *
 * If a request fails, the SDK retries the request at increasing intervals
 * and eventually drops data if the request cannot be completed.
 *
 * If zero is given as a maximum, no retries will be made for failed
 * requests.
 *
 * For a backoff factor of 1 second, and a maximum of 6 retries, the retry
 * delay interval should follow a pattern of [0, 1, 2, 4, 8, 16].
 *
 * See the
 * [specification](https://github.com/newrelic/newrelic-telemetry-sdk-specs/blob/master/communication.md#graceful-degradation)
 * for further details.
 *
 * @param config A client configuration.
 * @param retries The maximum number of retries for failed requests.
 */
void nrt_client_config_set_retries_max(nrt_client_config_t* config,
                                       uint32_t retries);

/**
 * @brief Configure the ingest host for traces.
 *
 * This overrides the default ingest host for traces to facilitate communication
 * with alternative New Relic backends.
 *
 * @param config A client configuration.
 * @param host The name of the ingest host.
 * @param port The port of the ingest host. Pass `0` to indicate usage of the
 * default port.
 */
void nrt_client_config_set_endpoint_traces(nrt_client_config_t* config,
                                           const char* host,
                                           uint16_t port);

/**
 * @brief Configure a product and version.
 *
 * The specified product and version will be appended to the `User-Agent`
 * header of payloads.
 *
 * See the
 * [specification](https://github.com/newrelic/newrelic-telemetry-sdk-specs/blob/master/communication.md#user-agent)
 * for further details.
 *
 * @param config A client configuration.
 * @param product A product name.
 * @param version A product version. Pass NULL to use no version.
 */
void nrt_client_config_set_product_info(nrt_client_config_t* config,
                                        const char* product,
                                        const char* version);

/**
 * @brief Configure the maximum of batches sent in one go.
 *
 * If the number of batches queued exceeds the maximum given here, the
 * additional batches will be dropped. This mechanism avoids accumulating
 * back pressure.
 *
 * @param config A client configuration.
 * @param queue_max The maximum queue size.
 */
void nrt_client_config_set_queue_max(nrt_client_config_t* config,
                                     size_t queue_max);

/**
 * @brief Destroy a client configuration.
 *
 * This will destroy a client configuration object and set the passed pointer to
 * NULL.
 *
 * @param config A client configuration.
 */
void nrt_client_config_destroy(nrt_client_config_t** config);

/**
 * @brief Create a new client.
 *
 * The passed configuration will be destroyed and the passed pointer will
 * always be set to NULL.
 *
 * @param config A client configuration.
 * @return A client.
 */
nrt_client_t* nrt_client_new(nrt_client_config_t** config);

/**
 * @brief Send a span batch.
 *
 * Put a span batch in the queue of the client. This function returns as soon
 * as the span batch was queued and doesn't wait for it to be sent
 * successfully.
 *
 * If the span batch is successfully queued, the client takes ownership of the
 * span batch. Otherwise the batch will be destroyed. The passed pointer will
 * always be set to NULL.
 *
 * @param client A client.
 * @param batch The span batch to be sent.
 * @return True if the span batch was successfully queued.
 */
bool nrt_client_send(nrt_client_t* client, nrt_span_batch_t** batch);

/**
 * @brief Shutdown a client.
 *
 * Shuts down the client, sends pending data and frees the client object. The
 * passed pointer will be set to NULL.
 *
 * @param client A client.
 */
void nrt_client_shutdown(nrt_client_t** client);

/**
 * @brief Destroy a client.
 *
 * Destroy the client, without making sure pending data is sent. The passed
 * pointer will be set to NULL.
 *
 * @param client A client.
 */
void nrt_client_destroy(nrt_client_t** client);

/**
 * A list of examples for Doxygen to cross-reference. If a function in
 * newrelic-telemetry-sdk.h appears in one of these examples, the example source
 * file appears under the "Examples" header.
 *
 * \example attributes.c
 * \example configuration.c
 * \example log.c
 * \example simple.c
 * \example span.c
 * \example trace_api.c
 */

#ifdef __cplusplus
}
#endif

#endif /* NEWRELIC_TELEMETRY_SDK */
