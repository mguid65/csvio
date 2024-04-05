cmake_minimum_required(VERSION 3.16)
project(csvio LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(cmake/utilities.cmake)

enable_warnings()

option(COVERAGE "Enable code coverage flags" Off)
option(SANITIZE "Enable sanitizer flags" Off)

option(CSVIO_BUILD_SAMPLE "Enable building of sample" On)
option(CSVIO_BUILD_TESTS "Enable building of tests" On)

if (COVERAGE)
    enable_coverage()
endif ()

if (SANITIZE)
    enable_sanitizer()
endif ()

set(CSVIO_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/csvio/csvio.hpp
)

add_library(csvio INTERFACE)
target_include_directories(csvio INTERFACE include)
target_sources(csvio INTERFACE ${DATA_TREE_HEADERS})
set_target_properties(csvio PROPERTIES LINKER_LANGUAGE CXX)


install(DIRECTORY include/ DESTINATION include)
install(TARGETS csvio DESTINATION lib)

if (CSVIO_BUILD_SAMPLE)
    add_executable(sample)
    target_sources(sample PRIVATE source/sample/main.cpp)
    target_link_libraries(sample PRIVATE csvio)
endif ()

if (CSVIO_BUILD_TESTS)
    include(CTest)
    enable_testing()

    if (CSVIO_BUILD_SAMPLE)
        add_test(NAME sample
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMAND sample)
    endif ()

    find_package(GTest REQUIRED)

    if (GTest_FOUND)
        add_subdirectory(test)
    endif ()
endif ()