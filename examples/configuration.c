/*
 * Copyright 2020 New Relic Corporation. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "newrelic-telemetry-sdk.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Create and destroy a configuration and use all options.
 */
int main() {
  const char* api_key = getenv("NEW_RELIC_API_KEY");

  if (!api_key) {
    fprintf(stderr, "NEW_RELIC_API_KEY not set\n");
    exit(1);
  }

  /* Destroy a NULL configuration. */
  nrt_client_config_t* cfg = NULL;
  nrt_client_config_destroy(&cfg);

  /* Initialize a configuration use all options. */
  cfg = nrt_client_config_new(api_key);
  assert(cfg);
  nrt_client_config_set_backoff_factor(cfg, 1000);
  nrt_client_config_set_retries_max(cfg, 5);
  nrt_client_config_set_endpoint_traces(cfg, "localhost", 31339);
  nrt_client_config_set_product_info(cfg, "Example", "1.0");
  nrt_client_config_set_queue_max(cfg, 200);
  nrt_client_config_destroy(&cfg);
  assert(NULL == cfg);

  /* Call with NULL values. */
  nrt_client_config_new(NULL);
  nrt_client_config_set_backoff_factor(NULL, 1000);
  nrt_client_config_set_retries_max(NULL, 5);
  nrt_client_config_set_endpoint_traces(NULL, NULL, 31339);
  nrt_client_config_set_product_info(NULL, NULL, NULL);
  nrt_client_config_set_queue_max(NULL, 200);
  nrt_client_config_destroy(NULL);
}
