#include "newrelic-telemetry-sdk.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef OS_WINDOWS
#include <windows.h>
#else
#include <sys/time.h>
#endif

/*
 * A dummy example that needs to be replaced once this library can actually do
 * something.
 */
int main() {
  const char* api_key = getenv("NEW_RELIC_API_KEY");

  if (!api_key) {
    fprintf(stderr, "NEW_RELIC_API_KEY not set\n");
    exit(1);
  }

  /* Initialize a configuration. */
  nrt_client_config_t* cfg = nrt_client_config_new(api_key);

  /* Initialize a new client with the given API key. */
  nrt_client_t* client = nrt_client_new(&cfg);

  /* Create an empty span batch */
  nrt_span_batch_t* batch = nrt_span_batch_new();

  /* Create a span and add it to the batch */
  {
#ifdef OS_WINDOWS
    SYSTEMTIME time;
    GetSystemTime(&time);
    nrt_time_t time_ms = (time.wSecond * 1000) + time.wMilliseconds;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    nrt_time_t time_ms = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
#endif

    nrt_span_t* span
        = nrt_span_new("e9f54a2c322d7578", "1b1bf29379951c1d", time_ms);
    nrt_span_set_name(span, "Root span");
    nrt_span_set_duration(span, 2000);
    nrt_span_set_parent_id(span, "parent_id");
    nrt_span_set_service_name(span, "Telemetry Application");

    nrt_attributes_t* attrs = nrt_attributes_new();
    nrt_attributes_set_int(attrs, "int", -6);
    nrt_attributes_set_uint(attrs, "uint", 6);
    nrt_attributes_set_double(attrs, "double", 3.14159);
    nrt_attributes_set_string(attrs, "string", "value");
    nrt_attributes_set_bool(attrs, "bool", true);
    nrt_span_set_attributes(span, &attrs);

    nrt_span_batch_record(batch, &span);
    nrt_span_batch_destroy(&batch);
  }

  /* Queue the span batch */
  nrt_client_send(client, &batch);
  assert(batch == NULL);

  /* Wait for the batch to be sent and shut down the client. */
  nrt_client_shutdown(&client);
  assert(client == NULL);
}
