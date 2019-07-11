#include <string>
#include <sstream>
#include <vector>
#include <iostream>
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



} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

