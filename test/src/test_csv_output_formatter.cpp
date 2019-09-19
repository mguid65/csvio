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

#include "csv_io.hpp"
#include "gtest/gtest.h"

namespace {

TEST(CSVOutputFormatterTest, SingleVectorBlankToStringEscaped) {
  std::vector<std::string> to_join{""};
  std::string expected{"\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::vector>::delim_join_escaped_fmt(to_join, ',', "\r\n"));
}

TEST(CSVOutputFormatterTest, JoinSampleVectorUnescapedToStringEscaped) {
  std::vector<std::string> to_join{"a", "b", "c", "d", "e"};
  std::string expected{"a,b,c,d,e\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::vector>::delim_join_escaped_fmt(to_join, ',', "\r\n"));
}

TEST(CSVOutputFormatterTest, SingleVectorBlankToStringUnescaped) {
  std::vector<std::string> to_join{""};
  std::string expected{"\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::vector>::delim_join_unescaped_fmt(to_join, ',', "\r\n"));
}

TEST(CSVOutputFormatterTest, JoinSampleVectorUnscapedToStringUnescaped) {
  std::vector<std::string> to_join{"a", "b", "c", "d", "e"};
  std::string expected{"a,b,c,d,e\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::vector>::delim_join_unescaped_fmt(to_join, ',', "\r\n"));
}

TEST(CSVOutputFormatterTest, JoinSampleVectorEscapedToStringEscaped) {
  std::vector<std::string> to_join{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  std::string expected{"\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::vector>::delim_join_escaped_fmt(to_join, ',', "\r\n"));
}

TEST(CSVOutputFormatterTest, JoinSampleVectorInFieldDelimiterEscapedToStringEscaped) {
  std::vector<std::string> to_join{",a", ",b", ",c", ",d", ",e"};
  std::string expected{"\",a\",\",b\",\",c\",\",d\",\",e\"\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::vector>::delim_join_escaped_fmt(to_join, ',', "\r\n"));
}

TEST(CSVOutputFormatterTest, JoinSampleVectorInFieldDelimiterEscapedToStringEscapedHardParse) {
  std::vector<std::string> to_join{"\"one\"", "tw\no", "\"th,r\nee\"", "\"fo\nu\"r\"", "5", "6",
                                   "7",       "8"};
  std::string expected{
      "\"\"\"one\"\"\",\"tw\n"
      "o\",\"\"\"th,r\n"
      "ee\"\"\",\"\"\"fo\n"
      "u\"\"r\"\"\",5,6,7,8\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::vector>::delim_join_escaped_fmt(to_join, ',', "\r\n"));
}

TEST(CSVOutputFormatterTest, SingleListBlankToStringEscaped) {
  std::list<std::string> to_join{""};
  std::string expected{"\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::list>::delim_join_escaped_fmt(to_join, ',', "\r\n"));
}

TEST(CSVOutputFormatterTest, JoinSampleListUnescapedToStringEscaped) {
  std::list<std::string> to_join{"a", "b", "c", "d", "e"};
  std::string expected{"a,b,c,d,e\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::list>::delim_join_escaped_fmt(to_join, ',', "\r\n"));
}

TEST(CSVOutputFormatterTest, SingleListBlankToStringUnescaped) {
  std::list<std::string> to_join{""};
  std::string expected{"\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::list>::delim_join_unescaped_fmt(to_join, ',', "\r\n"));
}

TEST(CSVOutputFormatterTest, JoinSampleListUnscapedToStringUnescaped) {
  std::list<std::string> to_join{"a", "b", "c", "d", "e"};
  std::string expected{"a,b,c,d,e\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::list>::delim_join_unescaped_fmt(to_join, ',', "\r\n"));
}

TEST(CSVOutputFormatterTest, JoinSampleListEscapedToStringEscaped) {
  std::list<std::string> to_join{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  std::string expected{"\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::list>::delim_join_escaped_fmt(to_join, ',', "\r\n"));
}

TEST(CSVOutputFormatterTest, JoinSampleListInFieldDelimiterEscapedToStringEscaped) {
  std::list<std::string> to_join{",a", ",b", ",c", ",d", ",e"};
  std::string expected{"\",a\",\",b\",\",c\",\",d\",\",e\"\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::list>::delim_join_escaped_fmt(to_join, ',', "\r\n"));
}

TEST(CSVOutputFormatterTest, JoinSampleListInFieldDelimiterEscapedToStringEscapedHardParse) {
  std::list<std::string> to_join{"\"one\"", "tw\no", "\"th,r\nee\"", "\"fo\nu\"r\"", "5", "6",
                                 "7",       "8"};
  std::string expected{
      "\"\"\"one\"\"\",\"tw\n"
      "o\",\"\"\"th,r\n"
      "ee\"\"\",\"\"\"fo\n"
      "u\"\"r\"\"\",5,6,7,8\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::list>::delim_join_escaped_fmt(to_join, ',', "\r\n"));
}

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
