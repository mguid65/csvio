/*
 * MIT License
 *
 * Copyright (c) 2019 Matthew Guidry
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <fstream>
#include <string>
#include <vector>
#include "csv_io.hpp"
#include "gtest/gtest.h"

namespace {
TEST(CSVReaderIfstream, ConstructorFromIfstream) {
  std::ifstream infile("data/test_data.csv");
  {
    csvio::util::CSVLineReader csv_line_reader(infile);
    csvio::CSVReader csv_reader(csv_line_reader);
    EXPECT_EQ(true, csv_reader.good());
    EXPECT_EQ(0u, csv_reader.lcount());
  }
  infile.close();
}

TEST(CSVReaderIfstream, ConstructorFromIfstreamWithHeader) {
  std::ifstream infile("data/test_data.csv");
  {
    csvio::util::CSVLineReader csv_line_reader(infile);
    csvio::CSVReader csv_reader(csv_line_reader, ',', true);

    std::vector<std::string> expected{"seq","name/first","age","city","pick","date"};
    EXPECT_EQ(true, csv_reader.good());
    EXPECT_EQ(1u, csv_reader.lcount());
    EXPECT_EQ(expected, csv_reader.get_header_names());
  }
  infile.close();
}

TEST(CSVReaderIfstream, ReadWholeFileNoHeader) {
  std::ifstream infile("data/test_data.csv");
  {
    csvio::util::CSVLineReader csv_line_reader(infile);
    csvio::CSVReader csv_reader(csv_line_reader);

    std::vector<std::string> expected{""};
    std::vector<std::string> row;

    while(csv_reader.good()) {
      row = csv_reader.read();
    }

    EXPECT_EQ(expected, row);
    EXPECT_EQ(102u, csv_reader.lcount());
  }
  infile.close();
}

TEST(CSVReaderIfstream, ReadWholeFileWithHeader) {
  std::ifstream infile("data/test_data.csv");
  {
    csvio::util::CSVLineReader csv_line_reader(infile);
    csvio::CSVReader csv_reader(csv_line_reader, ',', true);

    std::vector<std::string> expected_header{"seq","name/first","age","city","pick","date"};
    std::vector<std::string> expected{""};
    std::vector<std::string> row;

    while(csv_reader.good()) {
      row = csv_reader.read();
    }

    EXPECT_EQ(expected_header, csv_reader.get_header_names());
    EXPECT_EQ(expected, row);
    EXPECT_EQ(102u, csv_reader.lcount());
  }
  infile.close();
}

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

