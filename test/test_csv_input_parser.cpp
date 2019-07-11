
#include <string>
#include <string_view>
#include <vector>
#include <list>

#include "csv_io.hpp"
#include "gtest/gtest.h"

namespace {

TEST(CSVInputParserTest, SplitEmptyStringToVector) {
  std::string to_split{""};
  std::vector<std::string> expected{""};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::vector>::delim_split_naive(to_split, ','));
}

TEST(CSVInputParserTest, SplitSampleStringToVector) {
  std::string to_split{"a,b,c,d,e"};
  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::vector>::delim_split_naive(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscEmptySVToVector) {
  std::string_view to_split{""};
  std::vector<std::string> expected{""};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::vector>::delim_split_unescaped(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToVector) {
  std::string_view to_split{"\"a\",\"b\",\"c\""};
  std::vector<std::string> expected{"a","b","c"};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::vector>::delim_split_unescaped(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToVectorNoUnescape) {
  std::string_view to_split{"\"a\",\"b\",\"c\""};
  std::vector<std::string> expected{"\"a\"","\"b\"","\"c\""};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::vector>::delim_split_escaped(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToVectorWithCommas) {
  std::string_view to_split{"\"a,\",\"b,\",\"c,\""};
  std::vector<std::string> expected{"a,","b,","c,"};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::vector>::delim_split_unescaped(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToVectorNoUnescapeWithCommas) {
  std::string_view to_split{"\"a,\",\"b,\",\"c,\""};
  std::vector<std::string> expected{"\"a,\"","\"b,\"","\"c,\""};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::vector>::delim_split_escaped(to_split, ','));
}

TEST(CSVInputParserTest, SplitEmptyStringToList) {
  std::string to_split{""};
  std::vector<std::string> expected{""};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::vector>::delim_split_naive(to_split, ','));
}

TEST(CSVInputParserTest, SplitSampleStringToList) {
  std::string to_split{"a,b,c,d,e"};
  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::vector>::delim_split_naive(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscEmptySVToList) {
  std::string_view to_split{""};
  std::list<std::string> expected{""};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::list>::delim_split_unescaped(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToList) {
  std::string_view to_split{"\"a\",\"b\",\"c\""};
  std::list<std::string> expected{"a","b","c"};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::list>::delim_split_unescaped(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToListNoUnescape) {
  std::string_view to_split{"\"a\",\"b\",\"c\""};
  std::list<std::string> expected{"\"a\"","\"b\"","\"c\""};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::list>::delim_split_escaped(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToListWithCommas) {
  std::string_view to_split{"\"a,\",\"b,\",\"c,\""};
  std::list<std::string> expected{"a,","b,","c,"};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::list>::delim_split_unescaped(to_split, ','));
}

TEST(CSVInputParserTest, SplitEscSVToListNoUnescapeWithCommas) {
  std::string_view to_split{"\"a,\",\"b,\",\"c,\""};
  std::list<std::string> expected{"\"a,\"","\"b,\"","\"c,\""};
  EXPECT_EQ(expected, csvio::util::CSVInputParser<std::list>::delim_split_escaped(to_split, ','));
}


} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

