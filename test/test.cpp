/**
MIT License

Copyright (c) 2019 Matthew Guidry

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <cassert>
#include <chrono>
#include <string>
#include <vector>

#include "csv_io.hpp"

void print_vector(std::vector<std::string_view> v) {
  for (auto& s : v) {
    std::cout << s << ' ';
  }
  std::cout << '\n';
}

void test_SplitFunction() {
  std::cout << "Split Function Test:\n";
  std::string test_1{"abc,def,ghi,jkl"};
  std::string delimiter = ",";
  std::vector<std::string_view> expected{"abc", "def", "ghi", "jkl"};
  auto actual = VectorSplitSV::delimiter_split(test_1, delimiter);

  // print_vector(expected);
  // print_vector(actual);
  assert(actual == expected);
}

void test_CSVReader() {
  std::cout << "Reader Test:\n";
  std::string filename{"test.csv"};
  CSVReader<std::vector<std::string_view>> csv(filename);
  while (csv.good()) {
    auto v = csv.read();
    // print_vector(v);
  }
}

std::vector<std::string_view> test_values{"0", "1", "2", "3", "4"};

void test_CSVWriter() {
  std::cout << "Writer Test:\n";
  std::string filename{"testout.csv"};
  CSVWriter csv(filename);

  csv.write_header({"a", "b", "c", "d", "e"});
  csv.write_row({"0", "1", "2", "3", "4"});

  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 5; j++) {
      csv.push_back(test_values[j]);
    }
    csv.write_row();
    csv.next_row();
  }
}

void test_CSVWriterPerf() {
  std::cout << "Write Performance Test:\n";
  typedef std::chrono::high_resolution_clock Time;

  std::string filename{"testperf.csv"};
  CSVWriter csv(filename);

  auto t0 = Time::now();
  auto t1 = Time::now();

  t0 = Time::now();
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 5; j++) {
      csv.push_back(test_values[j]);
    }
    csv.write_row();
    csv.next_row();
  }
  t1 = Time::now();
  std::cout << "10 Row Write: "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count() << " ns\n";

  t0 = Time::now();
  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 5; j++) {
      csv.push_back(test_values[j]);
    }
    csv.write_row();
    csv.next_row();
  }
  t1 = Time::now();
  std::cout << "100 Row Write: "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count() << " ns\n";

  t0 = Time::now();
  for (int i = 0; i < 1000; i++) {
    for (int j = 0; j < 5; j++) {
      csv.push_back(test_values[j]);
    }
    csv.write_row();
    csv.next_row();
  }
  t1 = Time::now();
  std::cout << "1000 Row Write: "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count() << " ns\n";

  t0 = Time::now();
  for (int i = 0; i < 10000; i++) {
    for (int j = 0; j < 5; j++) {
      csv.push_back(test_values[j]);
    }
    csv.write_row();
    csv.next_row();
  }
  t1 = Time::now();
  std::cout << "10000 Row Write: "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count() << " ns\n";

  t0 = Time::now();
  for (int i = 0; i < 100000; i++) {
    for (int j = 0; j < 5; j++) {
      csv.push_back(test_values[j]);
    }
    csv.write_row();
    csv.next_row();
  }
  t1 = Time::now();
  std::cout << "100000 Row Write: "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count() << " ns\n";

  t0 = Time::now();
  for (int i = 0; i < 1000000; i++) {
    for (int j = 0; j < 5; j++) {
      csv.push_back(test_values[j]);
    }
    csv.write_row();
    csv.next_row();
  }
  t1 = Time::now();
  std::cout << "1000000 Row Write: "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count() << " ns\n";
}

int main() {
  test_SplitFunction();
  test_CSVReader();
  test_CSVWriter();

  test_CSVWriterPerf();
}
