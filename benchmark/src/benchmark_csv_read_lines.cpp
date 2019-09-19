#include <benchmark/benchmark.h>
#include <fstream>
#include "csv_io.hpp"

static void BM_CSVReadLines(benchmark::State& state) {
  std::ifstream infile("./data/CSV_READER_BENCHMARK_001.csv");
  csvio::util::CSVLineReader csv_line_reader(infile);
  csvio::CSVReader<std::vector> csv_reader(csv_line_reader);

  for (auto _ : state) {
    for (int i = 0; i < state.range(0); i++) {
      benchmark::DoNotOptimize(csv_reader.read());
    }
  }
  infile.close();
}

BENCHMARK(BM_CSVReadLines)->RangeMultiplier(2)->Range(8, 8 << 10);

BENCHMARK_MAIN();
