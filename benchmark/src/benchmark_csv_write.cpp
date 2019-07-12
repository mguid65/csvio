#include <benchmark/benchmark.h>
#include <fstream>
#include "csv_io.hpp"

static void BM_CSVWrite(benchmark::State& state) {
  std::ofstream outfile("CSV_WRITER_BENCHMARK_001.csv");
  csvio::util::CSVLineWriter csv_line_writer(outfile);
  csvio::CSVWriter<std::vector> csv_writer(csv_line_writer);

  std::vector<std::string> data{"sometext", "sometext", "sometext", "sometext"};
  for(auto _ : state) {
    for(int i = 0; i < state.range(0); i++) {
      csv_writer.write(data);
    }
  }
  outfile.close();
}

BENCHMARK(BM_CSVWrite)->RangeMultiplier(2)->Range(8, 8<<10);

BENCHMARK_MAIN();
