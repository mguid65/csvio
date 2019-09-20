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
#include "csv_io.hpp"
#include "gtest/gtest.h"

namespace {

TEST(CSVReaderTest, ConstructorFromLineReader) {
  std::istringstream instream("");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{""};
  EXPECT_EQ(expected, csv_reader.current());
  EXPECT_EQ(true, csv_reader.good());

  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadCSVHeader) {
  std::istringstream instream("a,b,c,d,e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr, ',', true);

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.get_header_names());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneCSVLine) {
  std::istringstream instream("a,b,c,d,e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadBlankCSVFields) {
  std::istringstream instream(",,,,");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"", "", "", "", ""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLine) {
  std::istringstream instream("\"a\",\"b\",\"c\",\"d\",\"e\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineWithEscapedQuotes) {
  std::istringstream instream(
      "\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesThreaded) {
  std::istringstream instream(
      "\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::vector, csvio::util::CSVLineReader, csvio::util::DelimSplitUnescapedThreaded<std::vector>> csv_reader(
      csv_lr, ',', false, true);

  std::vector<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneCSVLineAltDelim) {
  std::istringstream instream("a|b|c|d|e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr, '|');

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineAltDelim) {
  std::istringstream instream("\"a\"|\"b\"|\"c\"|\"d\"|\"e\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr, '|');

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltDelim) {
  std::istringstream instream(
      "\"\"\"a\"\"\"|\"\"\"b\"\"\"|\"\"\"c\"\"\"|\"\"\"d\"\"\"|\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr, '|');

  std::vector<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ConstructorFromLineReaderAltContainer) {
  std::istringstream instream("");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr);

  std::list<std::string> expected{""};
  EXPECT_EQ(expected, csv_reader.current());
  EXPECT_EQ(true, csv_reader.good());

  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadCSVHeaderAltContainer) {
  std::istringstream instream("a,b,c,d,e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr, ',', true);

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.get_header_names());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneCSVLineAltContainer) {
  std::istringstream instream("a,b,c,d,e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr);

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineAltContainer) {
  std::istringstream instream("\"a\",\"b\",\"c\",\"d\",\"e\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr);

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltContainer) {
  std::istringstream instream(
      "\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr);

  std::list<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltContainerThreaded) {
  std::istringstream instream(
      "\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list, csvio::util::CSVLineReader, csvio::util::DelimSplitUnescapedThreaded<std::list>> csv_reader(
      csv_lr, ',', false, true);

  std::list<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneCSVLineAltDelimAltContainer) {
  std::istringstream instream("a|b|c|d|e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr, '|');

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineAltDelimAltContainer) {
  std::istringstream instream("\"a\"|\"b\"|\"c\"|\"d\"|\"e\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr, '|');

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltDelimAltContainer) {
  std::istringstream instream(
      "\"\"\"a\"\"\"|\"\"\"b\"\"\"|\"\"\"c\"\"\"|\"\"\"d\"\"\"|\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr, '|');

  std::list<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}
}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
