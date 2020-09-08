[![Community Project header](https://github.com/newrelic/open-source-office/raw/master/examples/categories/images/Community_Project.png)](https://github.com/newrelic/open-source-office/blob/master/examples/categories/index.md#community-project)

# New Relic C Telemetry SDK

![Build status](https://github.com/newrelic/newrelic-telemetry-sdk-c/workflows/CI/badge.svg)
[![Release](https://img.shields.io/github/v/release/newrelic/newrelic-telemetry-sdk-c?include_prereleases&style=)](https://github.com/newrelic/newrelic-telemetry-sdk-c/releases/)

What is the New Relic C Telemetry SDK?

* It's a helper library that supports sending New Relic data from within your C/C++ application.
* It’s an example of "best practices" for sending us data.

[The Telemetry SDK](https://docs.newrelic.com/docs/telemetry-data-platform/get-started/capabilities/telemetry-sdks-send-custom-telemetry-data-new-relic) provides you, the end-user-programmer, with a `Client` that sends `Spans` to New Relic. Individual spans are collected together into batches (via a `SpanBatch` object), and clients send these batches.  It serves as a foundation for getting open-standards based telemetry data like [OpenCensus](https://opencensus.io/), [OpenTracing](https://opentracing.io/), and [OpenTelemetry](https://opentelemetry.io/) into New Relic. You can use this to build tracers/exporters, such as ones based on these open standards.

This SDK currently supports sending spans to the [Trace API](https://docs.newrelic.com/docs/understand-dependencies/distributed-tracing/trace-api/introduction-trace-api).

## Requirements

The C Telemetry SDK is a wrapper around the [Rust Telemetry
SDK](https://github.com/newrelic/newrelic-telemetry-sdk-rust). Minimal build
requirements are:

* CMake 3.0
* Rust 1.44

For running tests under Linux, valgrind is required.

The C Telemetry SDK aims to support any platform that is [supported by
Rust](https://doc.rust-lang.org/nightly/rustc/platform-support.html). It was
tested on 64 bit Linux (GNU GCC 7.5), and on 64 bit Windows 2019 (Visual Studio
2019).

## Building the C Telemetry SDK

The Rust Telemetry SDK is included into this project as a git submodule. Before
building the C Telemetry SDK, make sure git submodules are initialized by
running this command:

```
git submodule update --init
```

### Linux

For building the C Telemetry SDK on Linux, run the following commands:

```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

To build and run tests:

```
mkdir build && cd build
cmake -DENABLE_TESTS=on ..
make
make test
```

### Windows

For building the C Telemetry SDK on Windows, run the following commands:

```
mkdir build
cd build
cmake ..
cmake --build .
```

## Getting Started

In order to send telemetry data to New Relic APIs, you will need an Insert API key. to find out how to generate this key, see our [docs]([url](https://docs.newrelic.com/docs/apis/get-started/intro-apis/types-new-relic-api-keys)).

Refer to the [API documentation](include/newrelic-telemetry-sdk.h) and to the
[examples](examples).

This is a simple application that sends a single span:
```c
#include "newrelic-telemetry-sdk.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef OS_WINDOWS
#include <windows.h>
#else
#include <sys/time.h>
#endif

int main() {

  /* Obtain an API key from the environment. */
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
    nrt_span_set_name(span, "/index.html");
    nrt_span_set_duration(span, 2000);
    nrt_span_set_service_name(span, "Telemetry Application");

    nrt_attributes_t* attrs = nrt_attributes_new();
    nrt_attributes_set_int(attrs, "retries", 3);
    nrt_attributes_set_string(attrs, "username", "user");
    nrt_span_set_attributes(span, &attrs);

    nrt_span_batch_record(batch, &span);
  }

  /* Queue the span batch */
  nrt_client_send(client, &batch);

  /* Wait for the batch to be sent and shut down the client. */
  nrt_client_shutdown(&client);
}
```

## Find and use your data

Tips on how to find and query your data in New Relic:
- [Find trace/span data](https://docs.newrelic.com/docs/understand-dependencies/distributed-tracing/trace-api/introduction-trace-api#view-data)

For general querying information, see:
- [Query New Relic data](https://docs.newrelic.com/docs/using-new-relic/data/understand-data/query-new-relic-data)
- [Intro to NRQL](https://docs.newrelic.com/docs/query-data/nrql-new-relic-query-language/getting-started/introduction-nrql)

## Contributing

We encourage your contributions to improve the C Telemetry SDK! Keep in mind
when you submit your pull request, you'll need to sign the CLA via the 
click-through using CLA-Assistant. You only have to sign the CLA one time per
project. If you have any questions, or to execute our corporate CLA, required
if your contribution is on behalf of a company,  please drop us an email at
opensource@newrelic.com.

## License

The C Telemetry SDK is licensed under the [Apache 2.0](http://apache.org/licenses/LICENSE-2.0.txt) 
License.

The C Telemetry SDK also uses source code from third-party libraries. You can
find full details on which libraries are used and the terms under which they
are licensed in the [third-party notices document](THIRD_PARTY_NOTICES.md).

### Limitations

The New Relic Telemetry APIs are rate limited. Please reference the
documentation for [New Relic Metric API](https://docs.newrelic.com/docs/introduction-new-relic-metric-api) 
and [New Relic Trace API requirements and limits](https://docs.newrelic.com/docs/apm/distributed-tracing/trace-api/trace-api-general-requirements-limits)
on the specifics of the rate limits.
