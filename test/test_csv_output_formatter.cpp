
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
      expected, csvio::util::CSVOutputFormatter<std::vector>::delim_join_escaped_fmt(to_join, ','));
}

TEST(CSVOutputFormatterTest, JoinSampleVectorUnescapedToStringEscaped) {
  std::vector<std::string> to_join{"a", "b", "c", "d", "e"};
  std::string expected{"a,b,c,d,e\r\n"};
  EXPECT_EQ(
      expected, csvio::util::CSVOutputFormatter<std::vector>::delim_join_escaped_fmt(to_join, ','));
}

TEST(CSVOutputFormatterTest, SingleVectorBlankToStringUnescaped) {
  std::vector<std::string> to_join{""};
  std::string expected{"\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::vector>::delim_join_unescaped_fmt(to_join, ','));
}

TEST(CSVOutputFormatterTest, JoinSampleVectorUnscapedToStringUnescaped) {
  std::vector<std::string> to_join{"a", "b", "c", "d", "e"};
  std::string expected{"a,b,c,d,e\r\n"};
  EXPECT_EQ(
      expected,
      csvio::util::CSVOutputFormatter<std::vector>::delim_join_unescaped_fmt(to_join, ','));
}

TEST(CSVOutputFormatterTest, JoinSampleVectorEscapedToStringEscaped) {
  std::vector<std::string> to_join{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  std::string expected{"\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"\r\n"};
  EXPECT_EQ(
      expected, csvio::util::CSVOutputFormatter<std::vector>::delim_join_escaped_fmt(to_join, ','));
}

TEST(CSVOutputFormatterTest, JoinSampleVectorInFieldDelimiterEscapedToStringEscaped) {
  std::vector<std::string> to_join{",a", ",b", ",c", ",d", ",e"};
  std::string expected{"\",a\",\",b\",\",c\",\",d\",\",e\"\r\n"};
  EXPECT_EQ(
      expected, csvio::util::CSVOutputFormatter<std::vector>::delim_join_escaped_fmt(to_join, ','));
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
      expected, csvio::util::CSVOutputFormatter<std::vector>::delim_join_escaped_fmt(to_join, ','));
}

TEST(CSVOutputFormatterTest, SingleListBlankToStringEscaped) {
  std::list<std::string> to_join{""};
  std::string expected{"\r\n"};
  EXPECT_EQ(
      expected, csvio::util::CSVOutputFormatter<std::list>::delim_join_escaped_fmt(to_join, ','));
}

TEST(CSVOutputFormatterTest, JoinSampleListUnescapedToStringEscaped) {
  std::list<std::string> to_join{"a", "b", "c", "d", "e"};
  std::string expected{"a,b,c,d,e\r\n"};
  EXPECT_EQ(
      expected, csvio::util::CSVOutputFormatter<std::list>::delim_join_escaped_fmt(to_join, ','));
}

TEST(CSVOutputFormatterTest, SingleListBlankToStringUnescaped) {
  std::list<std::string> to_join{""};
  std::string expected{"\r\n"};
  EXPECT_EQ(
      expected, csvio::util::CSVOutputFormatter<std::list>::delim_join_unescaped_fmt(to_join, ','));
}

TEST(CSVOutputFormatterTest, JoinSampleListUnscapedToStringUnescaped) {
  std::list<std::string> to_join{"a", "b", "c", "d", "e"};
  std::string expected{"a,b,c,d,e\r\n"};
  EXPECT_EQ(
      expected, csvio::util::CSVOutputFormatter<std::list>::delim_join_unescaped_fmt(to_join, ','));
}

TEST(CSVOutputFormatterTest, JoinSampleListEscapedToStringEscaped) {
  std::list<std::string> to_join{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  std::string expected{"\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"\r\n"};
  EXPECT_EQ(
      expected, csvio::util::CSVOutputFormatter<std::list>::delim_join_escaped_fmt(to_join, ','));
}

TEST(CSVOutputFormatterTest, JoinSampleListInFieldDelimiterEscapedToStringEscaped) {
  std::list<std::string> to_join{",a", ",b", ",c", ",d", ",e"};
  std::string expected{"\",a\",\",b\",\",c\",\",d\",\",e\"\r\n"};
  EXPECT_EQ(
      expected, csvio::util::CSVOutputFormatter<std::list>::delim_join_escaped_fmt(to_join, ','));
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
      expected, csvio::util::CSVOutputFormatter<std::list>::delim_join_escaped_fmt(to_join, ','));
}

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
