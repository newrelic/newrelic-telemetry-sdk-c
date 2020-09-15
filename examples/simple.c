/*
 * Copyright 2020 New Relic Corporation. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "newrelic-telemetry-sdk.h"
#include <stdio.h>
#include <stdlib.h>

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

  /* Wait for the batch to be sent and shut down the client. */
  nrt_client_shutdown(&client);
}
