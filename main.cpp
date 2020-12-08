#include "csv_io.hpp"
#include <iostream>

int main (int argc, const char* argv[]) {
  if (argc < 0) {
    std::cerr << "USAGE: " << argv[0] << "<csv_file>\n";
    return 1;
  }

  std::ifstream infile(argv[1]);

  csvio::util::CSVLineReader line_reader(infile);
  csvio::CSVReader reader(line_reader);

  for (const auto& line : reader) {
    for (const auto& item : line) {
      std::cout << item << '\n';
    }
  }
}