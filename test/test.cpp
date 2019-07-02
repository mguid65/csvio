#include <cassert>
#include <vector>
#include <string>

#include "csv_io.hpp"

void print_vector(std::vector<std::string_view> v){
  for (auto & s: v) {
    std::cout << s << ' ';
  }
  std::cout << '\n';
}

void test_SplitFunction() {
  std::string test_1{"abc,def,ghi,jkl"};
  std::string delimiter=",";
  std::vector<std::string_view> expected{"abc", "def", "ghi", "jkl"};
  auto actual = VectorSplit::delimiter_split(test_1, delimiter);

  //print_vector(expected);
  //print_vector(actual);
  assert(VectorSplit::delimiter_split(test_1, delimiter) == expected);
}

void test_CSVReader() {
  std::string filename{"test.csv"};
  CSVReader<std::vector<std::string_view>> csv(filename);
  while (csv.good()) {
    auto v = csv.next();
    //print_vector(v);
  }
}

std::vector<std::string_view> test_values{"0", "1", "2", "3", "4"};

void test_CSVWriter() {
  std::string filename{"testout.csv"};
  CSVWriter csv(filename);

  csv.write_header({"a","b","c","d","e"});
  csv.write_row({"0", "1", "2", "3", "4"});

  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 5; j++) {
      csv.push_back(test_values[j]);
    }
    csv.write_row();
    csv.next_row();
  }
}

int main () {
  test_SplitFunction();
  test_CSVReader();
  test_CSVWriter();
}

