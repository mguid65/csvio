
#include <string>
#include <string_view>

#include "csv_io.hpp"
#include "gtest/gtest.h"

namespace {

TEST(SplitFunctionTest, SplitEmptyStringToVector) {
  std::string to_split{""};
  std::vector<std::string> expected{""};
  EXPECT_EQ(expected, VectorSplit::delim_split_naive(to_split, ','));
}

TEST(SplitFunctionTest, SplitSampleStringToVector) {
  std::string to_split{"a,b,c,d,e"};
  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, VectorSplit::delim_split_naive(to_split, ','));
}

TEST(SplitFunctionTest, SplitEscEmptySVToVector) {
  std::string_view to_split{""};
  std::vector<std::string> expected{""};
  EXPECT_EQ(expected, VectorSplit::delim_split_unescaped(to_split, ','));
}

TEST(SplitFunctionTest, SplitEscSVToVector) {
  std::string_view to_split{"\"a\",\"b\",\"c\""};
  std::vector<std::string> expected{"a","b","c"};
  EXPECT_EQ(expected, VectorSplit::delim_split_unescaped(to_split, ','));
}

TEST(SplitFunctionTest, SplitEscSVToVectorNoUnescape) {
  std::string_view to_split{"\"a\",\"b\",\"c\""};
  std::vector<std::string> expected{"\"a\"","\"b\"","\"c\""};
  EXPECT_EQ(expected, VectorSplit::delim_split_escaped(to_split, ','));
}

TEST(SplitFunctionTest, SplitEscSVToVectorWithCommas) {
  std::string_view to_split{"\"a,\",\"b,\",\"c,\""};
  std::vector<std::string> expected{"a,","b,","c,"};
  EXPECT_EQ(expected, VectorSplit::delim_split_unescaped(to_split, ','));
}

TEST(SplitFunctionTest, SplitEscSVToVectorNoUnescapeWithCommas) {
  std::string_view to_split{"\"a,\",\"b,\",\"c,\""};
  std::vector<std::string> expected{"\"a,\"","\"b,\"","\"c,\""};
  EXPECT_EQ(expected, VectorSplit::delim_split_escaped(to_split, ','));
}



} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

