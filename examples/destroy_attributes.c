#include "newrelic-telemetry-sdk.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Destroy an attributes collection.
 */
int main() {
  nrt_attributes_t* attrs = nrt_attributes_new();
  nrt_attributes_set_int(attrs, "int", -6);
  nrt_attributes_set_uint(attrs, "uint", 6);
  nrt_attributes_set_double(attrs, "double", 3.14159);
  nrt_attributes_set_string(attrs, "string", "value");
  nrt_attributes_set_bool(attrs, "bool", true);
  nrt_attributes_destroy(&attrs);
}
