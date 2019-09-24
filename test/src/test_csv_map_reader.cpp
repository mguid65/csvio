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
 */

#include <map>
#include <sstream>
#include <string>
#include <vector>
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

  std::map<std::string, std::string> expected{
      {"a", "1"}, {"b", "2"}, {"c", "3"}, {"d", "4"}, {"e", "5"}};

  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadBlankCSVFields) {
  std::istringstream instream("a,b,c,d,e\r\n,,,,\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader<std::map> csv_map_reader(csv_lr);

  std::map<std::string, std::string> expected{
      {"a", ""}, {"b", ""}, {"c", ""}, {"d", ""}, {"e", ""}};
  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLine) {
  std::istringstream instream("a,b,c,d,e\r\n\"a\",\"b\",\"c\",\"d\",\"e\"\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader csv_map_reader(csv_lr);

  std::map<std::string, std::string> expected{
      {"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"}, {"e", "e"}};
  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineWithEscapedQuotes) {
  std::istringstream instream(
      "a,b,c,d,e\r\n"
      "\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader csv_map_reader(csv_lr);

  std::map<std::string, std::string> expected{
      {"a", "\"a\""}, {"b", "\"b\""}, {"c", "\"c\""}, {"d", "\"d\""}, {"e", "\"e\""}};
  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadOneCSVLineAltDelim) {
  std::istringstream instream("a|b|c|d|e\r\n1|2|3|4|5\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader csv_map_reader(csv_lr, '|');

  std::map<std::string, std::string> expected{
      {"a", "1"}, {"b", "2"}, {"c", "3"}, {"d", "4"}, {"e", "5"}};
  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineAltDelim) {
  std::istringstream instream("a|b|c|d|e\r\n\"1\"|\"2\"|\"3\"|\"4\"|\"5\"\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader csv_map_reader(csv_lr, '|');

  std::map<std::string, std::string> expected{
      {"a", "1"}, {"b", "2"}, {"c", "3"}, {"d", "4"}, {"e", "5"}};
  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltDelim) {
  std::istringstream instream(
      "1|2|3|4|5\r\n"
      "\"\"\"a\"\"\"|\"\"\"b\"\"\"|\"\"\"c\"\"\"|\"\"\"d\"\"\"|\"\"\"e\"\"\"\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader csv_map_reader(csv_lr, '|');

  std::map<std::string, std::string> expected{
      {"1", "\"a\""}, {"2", "\"b\""}, {"3", "\"c\""}, {"4", "\"d\""}, {"5", "\"e\""}};
  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ConstructorFromLineReaderAltContainer) {
  std::istringstream instream("");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader<std::unordered_map> csv_map_reader(csv_lr);

  std::unordered_map<std::string, std::string> expected;
  EXPECT_EQ(expected, csv_map_reader.current());
  EXPECT_EQ(false, csv_map_reader.good());

  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadCSVAltContainer) {
  std::istringstream instream("a,b,c,d,e\r\n1,2,3,4,5\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader<std::unordered_map> csv_map_reader(csv_lr, ',');

  std::unordered_map<std::string, std::string> expected{
      {"a", "1"}, {"b", "2"}, {"c", "3"}, {"d", "4"}, {"e", "5"}};
  std::vector<std::string> expected_header{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected_header, csv_map_reader.get_header_names());
  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineAltContainer) {
  std::istringstream instream("a,b,c,d,e\r\n\"a\",\"b\",\"c\",\"d\",\"e\"\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader<std::unordered_map> csv_map_reader(csv_lr);

  std::unordered_map<std::string, std::string> expected{
      {"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"}, {"e", "e"}};
  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltContainer) {
  std::istringstream instream(
      "a,b,c,d,e\r\n"
      "\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader<std::unordered_map> csv_map_reader(csv_lr);

  std::unordered_map<std::string, std::string> expected{
      {"a", "\"a\""}, {"b", "\"b\""}, {"c", "\"c\""}, {"d", "\"d\""}, {"e", "\"e\""}};
  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltContainerThreaded) {
  using CSVMapThreadedEscReader= csvio::CSVMapReader<std::unordered_map,
                      csvio::util::CSVLineReader,
                      csvio::util::MapDelimSplitUnescaped<std::unordered_map>>;
  std::istringstream instream(
      "a,b,c,d,e\r\n"
      "\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  CSVMapThreadedEscReader csv_map_reader(csv_lr, ',');

  std::unordered_map<std::string, std::string> expected{
      {"a", "\"a\""}, {"b", "\"b\""}, {"c", "\"c\""}, {"d", "\"d\""}, {"e", "\"e\""}};
  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadOneCSVLineAltDelimAltContainer) {
  std::istringstream instream("a|b|c|d|e\r\na|b|c|d|e\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader<std::unordered_map> csv_map_reader(csv_lr, '|');

  std::unordered_map<std::string, std::string> expected{
      {"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"}, {"e", "e"}};
  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineAltDelimAltContainer) {
  std::istringstream instream("a|b|c|d|e\r\n\"a\"|\"b\"|\"c\"|\"d\"|\"e\"\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader<std::unordered_map> csv_map_reader(csv_lr, '|');

  std::unordered_map<std::string, std::string> expected{
      {"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"}, {"e", "e"}};
  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

TEST(CSVMapReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltDelimAltContainer) {
  std::istringstream instream(
      "a|b|c|d|e\r\n\"\"\"a\"\"\"|\"\"\"b\"\"\"|\"\"\"c\"\"\"|\"\"\"d\"\"\"|\"\"\"e\"\"\"\r\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVMapReader<std::unordered_map> csv_map_reader(csv_lr, '|');

  std::unordered_map<std::string, std::string> expected{
      {"a", "\"a\""}, {"b", "\"b\""}, {"c", "\"c\""}, {"d", "\"d\""}, {"e", "\"e\""}};
  EXPECT_EQ(expected, csv_map_reader.read());
  EXPECT_EQ(false, csv_map_reader.good());
}

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
