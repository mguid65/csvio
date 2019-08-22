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

#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "csv_io.hpp"
#include "gtest/gtest.h"

namespace {

TEST(CSVMapReaderTest, ConstructorFromLineReader) {
  std::istringstream instream("");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader<std::map> csv_map_reader(csv_lr);

  std::map<std::string, std::string> expected;
  EXPECT_EQ(expected, csv_map_reader.current());
  EXPECT_EQ(false, csv_map_reader.good());

  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadOneCSVLine) {
  std::istringstream instream("a,b,c,d,e\r\n1,2,3,4,5\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader<std::map> csv_map_reader(csv_lr);

  std::map<std::string, std::string> expected{{"a","1"}, {"b","2"}, {"c","3"}, {"d","4"}, {"e","5"}};

  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(true, csv_map_reader.good());
}

/**
TEST(CSVMapReaderTest, ReadBlankCSVFields) {
  std::istringstream instream(",,,,");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"", "", "", "", ""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}


TEST(CSVMapReaderTest, ReadOneEscapedCSVLine) {
  std::istringstream instream("\"a\",\"b\",\"c\",\"d\",\"e\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineWithEscapedQuotes) {
  std::istringstream instream(
      "\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesThreaded) {
  std::istringstream instream(
      "\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::vector> csv_reader(csv_lr, ',', false, true, csvio::util::CSVInputParser<std::vector>::delim_split_unescaped_threaded);

  std::vector<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ReadOneCSVLineAltDelim) {
  std::istringstream instream("a|b|c|d|e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr, '|');

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineAltDelim) {
  std::istringstream instream("\"a\"|\"b\"|\"c\"|\"d\"|\"e\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr, '|');

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltDelim) {
  std::istringstream instream(
      "\"\"\"a\"\"\"|\"\"\"b\"\"\"|\"\"\"c\"\"\"|\"\"\"d\"\"\"|\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr, '|');

  std::vector<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ConstructorFromLineReaderAltContainer) {
  std::istringstream instream("");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr);

  std::list<std::string> expected{""};
  EXPECT_EQ(expected, csv_reader.current());
  EXPECT_EQ(true, csv_reader.good());

  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ReadCSVHeaderAltContainer) {
  std::istringstream instream("a,b,c,d,e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr, ',', true);

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.get_header_names());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ReadOneCSVLineAltContainer) {
  std::istringstream instream("a,b,c,d,e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr);

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineAltContainer) {
  std::istringstream instream("\"a\",\"b\",\"c\",\"d\",\"e\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr);

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltContainer) {
  std::istringstream instream(
      "\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr);

  std::list<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltContainerThreaded) {
  std::istringstream instream(
      "\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr, ',', false, true, csvio::util::CSVInputParser<std::list>::delim_split_unescaped_threaded);

  std::list<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ReadOneCSVLineAltDelimAltContainer) {
  std::istringstream instream("a|b|c|d|e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr, '|');

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineAltDelimAltContainer) {
  std::istringstream instream("\"a\"|\"b\"|\"c\"|\"d\"|\"e\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr, '|');

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltDelimAltContainer) {
  std::istringstream instream(
      "\"\"\"a\"\"\"|\"\"\"b\"\"\"|\"\"\"c\"\"\"|\"\"\"d\"\"\"|\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr, '|');

  std::list<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}
*/

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
