#include "newrelic-telemetry-sdk.h"
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
  const char *api_key = getenv("NEW_RELIC_API_KEY");

  if (!api_key) {
    fprintf(stderr, "NEW_RELIC_API_KEY not set\n");
    exit(1);
  }

  /* Initialize a new sender with the given API key. */
  nrt_sender_t *sender = nrt_sender_new(api_key);

  /* Create an empty span batch */
  nrt_span_batch_t *batch = nrt_span_batch_new();

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

    nrt_span_t *span =
        nrt_span_new("e9f54a2c322d7578", "1b1bf29379951c1d", NULL);
    nrt_span_set_name(span, "Root span");
    nrt_span_set_service_name(span, "Telemetry Application");
    nrt_span_set_timestamp(span, time_ms);
    nrt_span_set_duration(span, 2000);

    nrt_span_batch_record(batch, &span);
  }

  /* Queue the span batch */
  nrt_sender_send(sender, &batch);

  /* Wait for the batch to be sent and shut down the sender. */
  nrt_sender_shutdown(&sender);
}
