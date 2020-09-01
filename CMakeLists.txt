cmake_minimum_required(VERSION 3.0)

project(newrelic_telemetry_sdk_c)

#
# This includes the Rust/cargo integration for CMake.
#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(CMakeCargo)

enable_language(Rust)

#
# Handle command line flags
#
option(ENABLE_EXAMPLES "Whether to build examples" OFF)
option(ENABLE_TESTS "Whether to run tests" OFF)

if(ENABLE_TESTS)
    set(ENABLE_EXAMPLES on)
endif()

#
# Build the C wrapper around the Rust Telemetry SDK.
#
cargo_build(NAME newrelic_telemetry_sdk_c)

#
# Add the include directories
#
include_directories(include)

#
# Platform specific setup
#
if (WIN32)
    add_definitions(-DOS_WINDOWS)

    set(OS_LIBS Crypt32 Ncrypt Secur32 Userenv Ws2_32)
else()
    find_package(Threads)
    find_package(OpenSSL)
    find_package(ZLIB)

    set(OS_LIBS dl m ${CMAKE_THREAD_LIBS_INIT} ${ZLIB_LIBRARIES} ${OPENSSL_LIBRARIES})
endif()

#
# Build examples
#
set(EXAMPLES simple configuration trace_api attributes)

if (ENABLE_EXAMPLES)
    foreach (EXAMPLE ${EXAMPLES})
        add_executable(example_${EXAMPLE} examples/${EXAMPLE}.c)
        target_link_libraries(example_${EXAMPLE} newrelic_telemetry_sdk_c ${OS_LIBS})
    endforeach()
endif() 

#
# Add tests
#
# Under Linux examples are run under valgrind, under Windows examples are run
# without memory checker.
#
if (ENABLE_EXAMPLES AND ENABLE_TESTS)
    enable_testing()

    foreach (EXAMPLE ${EXAMPLES})
        if (WIN32)
            add_test(NAME test_${EXAMPLE}
                     COMMAND $<TARGET_FILE:example_${EXAMPLE}>)
        elseif (APPLE)
            add_test(NAME test_${EXAMPLE}
                     COMMAND $<TARGET_FILE:example_${EXAMPLE}>)
        else ()
            add_test(NAME test_${EXAMPLE}
                     COMMAND valgrind --leak-check=full 
                                      --error-exitcode=1 
                                      --suppressions=${CMAKE_SOURCE_DIR}/examples/valgrind-suppressions
                                      $<TARGET_FILE:example_${EXAMPLE}>)
        endif() 
    endforeach()
endif() 
