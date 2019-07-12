include_directories(${benchmark_csvio_SOURCE_DIR}/../include)
add_executable(benchmark_csv_write benchmark_csv_write.cpp)
TARGET_LINK_LIBRARIES(benchmark_csv_write benchmark pthread)

