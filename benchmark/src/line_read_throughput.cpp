#include <iostream>

#include "csv_io.hpp"

#include "benchmark/benchmark.h"

int main() {
  const int line_byte_size = 37;

  std::ifstream infile("./data/CSV_READER_BENCHMARK_001.csv");
  csvio::util::CSVLineReader csv_line_reader(infile);
  csvio::CSVReader<std::vector> csv_reader(csv_line_reader);

  auto t1 = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 1000000; i++) {
    benchmark::DoNotOptimize(csv_reader.read());
  }

  auto t2 = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
  double time_in_sec = static_cast<double>(duration) / static_cast<double>(1e9);
  int size_in_bytes = 1000000 * line_byte_size;
  double size_in_MB = size_in_bytes / 1e6;

  std::cout << "Bytes Read         : " << size_in_bytes << '\n'
            << "Time(nanos)        : " << duration << '\n'
            << "Throughput(Megabytes/sec) : " << (size_in_MB / time_in_sec) << '\n';
}
