cmake_minimum_required (VERSION 3.5.1)

project (test_csvio CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option (BENCHMARK_DEBUG_MODE "Enable benchmarking the debug version of csvio." OFF)

if(BENCHMARK_DEBUG_MODE)
  set(FLAGS "-O0 -ggdb3")
else()
  set(FLAGS "-O3")
endif()

set(CMAKE_CXX_FLAGS "-std=c++17 ${FLAGS}")

add_subdirectory(src)
