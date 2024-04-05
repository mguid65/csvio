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

#include <list>
#include <string>
#include <string_view>
#include <vector>

#include "csvio/csvio.hpp"
#include "gtest/gtest.h"

namespace {

namespace csvutil = csvio::util;

TEST(CSVInputParserTest, SplitEmptyStringToVector) {
  std::string to_split{""};
  std::vector<std::string> expected{""};
  csvutil::DelimSplitNaive<std::vector> parser;

  EXPECT_EQ(expected, parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitSampleStringToVector) {
  std::string to_split{"a,b,c,d,e"};
  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  csvutil::DelimSplitNaive<std::vector> parser;

  EXPECT_EQ(expected, parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscEmptySVToVector) {
  std::string_view to_split{""};
  std::vector<std::string> expected{""};
  csvutil::DelimSplitUnescaped<std::vector> parser;

  EXPECT_EQ(
      expected, parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToVector) {
  std::string_view to_split{"\"a\",\"b\",\"c\""};
  std::vector<std::string> expected{"a", "b", "c"};
  csvutil::DelimSplitUnescaped<std::vector> parser;

  EXPECT_EQ(
      expected, parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToVectorThreaded) {
  std::string_view to_split{"\"a\",\"b\",\"c\""};
  std::vector<std::string> expected{"a", "b", "c"};
  csvutil::DelimSplitUnescapedThreaded<std::vector> parser;

  EXPECT_EQ(
      expected,
      parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToVectorNoUnescape) {
  std::string_view to_split{"\"a\",\"b\",\"c\""};
  std::vector<std::string> expected{"\"a\"", "\"b\"", "\"c\""};
  csvutil::DelimSplitEscaped<std::vector> parser;

  EXPECT_EQ(expected, parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToVectorWithCommas) {
  std::string_view to_split{"\"a,\",\"b,\",\"c,\""};
  std::vector<std::string> expected{"a,", "b,", "c,"};
  csvutil::DelimSplitUnescaped<std::vector> parser;

  EXPECT_EQ(
      expected, parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToVectorNoUnescapeWithCommas) {
  std::string_view to_split{"\"a,\",\"b,\",\"c,\""};
  std::vector<std::string> expected{"\"a,\"", "\"b,\"", "\"c,\""};
  csvutil::DelimSplitEscaped<std::vector> parser;

  EXPECT_EQ(expected, parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitEmptyStringToList) {
  std::string to_split{""};
  std::list<std::string> expected{""};
  csvutil::DelimSplitNaive<std::list> parser;

  EXPECT_EQ(expected, parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitSampleStringToList) {
  std::string to_split{"a,b,c,d,e"};
  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  csvutil::DelimSplitNaive<std::list> parser;

  EXPECT_EQ(expected, parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscEmptySVToList) {
  std::string_view to_split{""};
  std::list<std::string> expected{""};
  csvutil::DelimSplitUnescaped<std::list> parser;

  EXPECT_EQ(expected, parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToList) {
  std::string_view to_split{"\"a\",\"b\",\"c\""};
  std::list<std::string> expected{"a", "b", "c"};
  csvutil::DelimSplitUnescaped<std::list> parser;

  EXPECT_EQ(expected, parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToListNoUnescape) {
  std::string_view to_split{"\"a\",\"b\",\"c\""};
  std::list<std::string> expected{"\"a\"", "\"b\"", "\"c\""};
  csvutil::DelimSplitEscaped<std::list> parser;

  EXPECT_EQ(expected, parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToListWithCommas) {
  std::string_view to_split{"\"a,\",\"b,\",\"c,\""};
  std::list<std::string> expected{"a,", "b,", "c,"};
  csvutil::DelimSplitUnescaped<std::list> parser;

  EXPECT_EQ(expected, parser(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToListNoUnescapeWithCommas) {
  std::string_view to_split{"\"a,\",\"b,\",\"c,\""};
  std::list<std::string> expected{"\"a,\"", "\"b,\"", "\"c,\""};
  csvutil::DelimSplitEscaped<std::list> parser;

  EXPECT_EQ(expected, parser(to_split, ','));
}

}  // namespace