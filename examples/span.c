/*
 * Copyright 2020 New Relic Corporation. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

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

  /* Initialize a span and add attributes */
  span = nrt_span_new("span_id", "trace_id", time_ms);
  assert(span);
  nrt_span_set_id(span, "span_id2");
  nrt_span_set_trace_id(span, "trace_id2");
  nrt_span_set_timestamp(span, time_ms);
  nrt_span_set_name(span, "Root span");
  nrt_span_set_duration(span, 2000);
  nrt_span_set_parent_id(span, "parent_id");
  nrt_span_set_service_name(span, "Telemetry Application");

  nrt_attributes_t* attrs = nrt_attributes_new();
  assert(attrs);
  nrt_attributes_set_int(attrs, "int", -6);
  nrt_attributes_set_uint(attrs, "uint", 6);
  nrt_attributes_set_double(attrs, "double", 3.14159);
  nrt_attributes_set_string(attrs, "string", "value");
  nrt_attributes_set_bool(attrs, "bool", true);
  nrt_span_set_attributes(span, &attrs);
  assert(NULL == attrs);

  nrt_span_destroy(&span);
  assert(NULL == span);

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
