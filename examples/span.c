#include "newrelic-telemetry-sdk.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef OS_WINDOWS
#include <windows.h>
#else
#include <sys/time.h>
#endif

/*
 * Exercise a span.
 */
int main() {

#ifdef OS_WINDOWS
  SYSTEMTIME time;
  GetSystemTime(&time);
  nrt_time_t time_ms = (time.wSecond * 1000) + time.wMilliseconds;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  nrt_time_t time_ms = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
#endif

  /* Destroy a NULL span. */
  nrt_span_t* span = NULL;
  nrt_span_destroy(&span);

  /* Initialize and destroy span */
  span = nrt_span_new("span_id", "trace_id", time_ms);
  nrt_span_set_id(span, "span_id2");
  nrt_span_set_trace_id(span, "trace_id2");
  nrt_span_set_timestamp(span, time_ms);
  nrt_span_set_name(span, "Root span");
  nrt_span_set_duration(span, 2000);
  nrt_span_set_parent_id(span, "parent_id");
  nrt_span_set_service_name(span, "Telemetry Application");
  nrt_span_destroy(&span);

  /* Call with NULL parameters */
  nrt_span_set_id(NULL, NULL);
  nrt_span_set_trace_id(NULL, NULL);
  nrt_span_set_timestamp(NULL, time_ms);
  nrt_span_set_name(NULL, NULL);
  nrt_span_set_duration(NULL, 2000);
  nrt_span_set_parent_id(NULL, NULL);
  nrt_span_set_service_name(NULL, NULL);

  nrt_span_destroy(NULL);
}