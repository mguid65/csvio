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

#include "csvio/csvio.hpp"
#include "gtest/gtest.h"

namespace {

TEST(CSVSimpleLineReaderTest, ConstructorFromIStream) {
  std::istringstream data("");
  csvio::util::CSVSimpleLineReader csv_lr(data);

  EXPECT_EQ(0u, csv_lr.lcount());
  EXPECT_EQ(true, csv_lr.good());
  EXPECT_EQ("", csv_lr.readline());
}

TEST(CSVSimpleLineReaderTest, ReadOneBlankLine) {
  std::istringstream data("");
  csvio::util::CSVSimpleLineReader csv_lr(data);

  EXPECT_EQ("", csv_lr.readline());
  EXPECT_EQ(1u, csv_lr.lcount());
  EXPECT_EQ(false, csv_lr.good());
}

TEST(CSVSimpleLineReaderTest, ReadOneSampleCSVLine) {
  std::istringstream data("1,1,1,1,1,1,1,1\n");
  csvio::util::CSVSimpleLineReader csv_lr(data);

  EXPECT_EQ("1,1,1,1,1,1,1,1", csv_lr.readline());
  EXPECT_EQ(1u, csv_lr.lcount());
  EXPECT_EQ(false, csv_lr.good());
}

TEST(CSVSimpleLineReaderTest, ReadOneSampleNoNewline) {
  std::istringstream data("1,1,1,1,1,1,1,1");
  csvio::util::CSVSimpleLineReader csv_lr(data);

  EXPECT_EQ("1,1,1,1,1,1,1,1", csv_lr.readline());
  EXPECT_EQ(1u, csv_lr.lcount());
  EXPECT_EQ(false, csv_lr.good());
}

TEST(CSVSimpleLineReaderTest, CheckGood) {
  std::istringstream data("1,1,1,1,1,1,1,1\n");
  csvio::util::CSVSimpleLineReader csv_lr(data);

  EXPECT_EQ("1,1,1,1,1,1,1,1", csv_lr.readline());
  EXPECT_EQ("", csv_lr.readline());
  EXPECT_EQ(2u, csv_lr.lcount());  // not sure if I should let this be 1 or 2
  EXPECT_EQ(false, csv_lr.good());
}

TEST(CSVSimpleLineReaderTest, ReadTwoSampleCSVLines) {
  std::istringstream data(
      "1,1,1,1,1,1,1,1\n"
      "2,2,2,2,2,2,2,2\n");
  csvio::util::CSVSimpleLineReader csv_lr(data);

  EXPECT_EQ("1,1,1,1,1,1,1,1", csv_lr.readline());
  EXPECT_EQ("2,2,2,2,2,2,2,2", csv_lr.readline());
  EXPECT_EQ(2u, csv_lr.lcount());
  EXPECT_EQ(false, csv_lr.good());
}

TEST(CSVSimpleLineReaderTest, ReadTwoLinesCRLF) {
  std::istringstream data(
      "1,1,1,1,1,1,1,1\r\n"
      "2,2,2,2,2,2,2,2\r\n");
  csvio::util::CSVSimpleLineReader csv_lr(data);

  EXPECT_EQ("1,1,1,1,1,1,1,1\r", csv_lr.readline());
  EXPECT_EQ("2,2,2,2,2,2,2,2\r", csv_lr.readline());
  EXPECT_EQ(2u, csv_lr.lcount());
  EXPECT_EQ(false, csv_lr.good());
}

}  // namespace
