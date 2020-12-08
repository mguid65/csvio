cmake_minimum_required(VERSION 3.15)
project(csvio)

set(CMAKE_CXX_STANDARD 17)

add_library(csvio INTERFACE)
target_include_directories(csvio INTERFACE include)

add_executable(main main.cpp)
target_link_libraries(main PUBLIC csvio)
