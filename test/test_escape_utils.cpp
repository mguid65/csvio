#include <string>

#include "csv_io.hpp"
#include "gtest/gtest.h"

namespace {

TEST(EscapeTest, EscapeEmptyString) {
  std::string to_escape{""};
  EXPECT_EQ("", csvio::util::escape(to_escape));
}

TEST(EscapeTest, UnescapeEmptyString) {
  std::string to_unescape{""};
  EXPECT_EQ("", csvio::util::unescape(to_unescape));
}

TEST(EscapeTest, EscapeStringWithQuote) {
  std::string to_escape{"some\"value"};
  EXPECT_EQ("\"some\"\"value\"", csvio::util::escape(to_escape));
}

TEST(EscapeTest, UnescapeStringStringWithQuote) {
  std::string to_unescape{"\"some\"\"value\""};
  EXPECT_EQ("some\"value", csvio::util::unescape(to_unescape));
}

TEST(EscapeTest, EscapeStringWithComma) {
  std::string to_escape{"some,value"};
  EXPECT_EQ("\"some,value\"", csvio::util::escape(to_escape));
}

TEST(EscapeTest, EscapeStringAltDelimiter) {
  std::string to_escape{"some|value"};
  EXPECT_EQ("\"some|value\"", csvio::util::escape(to_escape, '|'));
}

TEST(EscapeTest, EscapeStringForceEscape) {
  std::string to_escape{"somevalue"};
  EXPECT_EQ("\"somevalue\"", csvio::util::escape(to_escape, ',', true));
}

} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

