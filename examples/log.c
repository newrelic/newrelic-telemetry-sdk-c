#include "newrelic-telemetry-sdk.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * An example that initializes a logger and triggers a log message.
 */
int main() {
  const char* api_key = getenv("NEW_RELIC_API_KEY");

  if (!api_key) {
    fprintf(stderr, "NEW_RELIC_API_KEY not set\n");
    exit(1);
  }

  /* Initialize a logger with NULL. */
  nrt_log_init(NRT_LOG_DEBUG, NULL);

  /* Initialize a logger. */
  nrt_log_init(NRT_LOG_DEBUG, "./nrt.log");

  /* Initialize a configuration with an invalid host. */
  nrt_client_config_t* cfg = nrt_client_config_new(api_key);
  nrt_client_config_set_endpoint_traces(cfg, "host:80", 80);

  /* Initialize a new client with the given API key. */
  nrt_client_t* client = nrt_client_new(&cfg);

  /* Wait for the batch to be sent and shut down the client. */
  nrt_client_shutdown(&client);
}
