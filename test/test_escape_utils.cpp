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

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
