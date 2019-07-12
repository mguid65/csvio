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

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "csv_io.hpp"
#include "gtest/gtest.h"

namespace {

TEST(CSVWriterTest, WriteVectorOfEmptyString) {
  std::ostringstream outstream;
  csvio::util::CSVLineWriter csv_lw(outstream);
  csvio::CSVWriter<std::vector> csv_writer(csv_lw);

  csv_writer.write({""});
  EXPECT_EQ("\r\n", outstream.str());
  EXPECT_EQ(true, csv_writer.good());
}

TEST(CSVWriterTest, WriteVectorOfSampleStrings) {
  std::ostringstream outstream;
  csvio::util::CSVLineWriter csv_lw(outstream);
  csvio::CSVWriter<std::vector> csv_writer(csv_lw);

  csv_writer.write({"a", "b", "c", "d"});
  EXPECT_EQ("a,b,c,d\r\n", outstream.str());
  EXPECT_EQ(true, csv_writer.good());
}

TEST(CSVWriterTest, WriteVectorOfQuotedSampleStrings) {
  std::ostringstream outstream;
  csvio::util::CSVLineWriter csv_lw(outstream);
  csvio::CSVWriter<std::vector> csv_writer(csv_lw);

  csv_writer.write({"\"a\"", "\"b\"", "\"c\"", "\"d\""});
  EXPECT_EQ("\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\"\r\n", outstream.str());
  EXPECT_EQ(true, csv_writer.good());
}

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
