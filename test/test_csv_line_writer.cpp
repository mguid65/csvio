#include <sstream>

#include "csv_io.hpp"
#include "gtest/gtest.h"

namespace {

TEST(CSVLineWriterTest, ConstructorFromOStream) {
  std::ostringstream data;
  csvio::util::CSVLineWriter csv_lw(data);

  EXPECT_EQ(0u, csv_lw.lcount());
  EXPECT_EQ(true, csv_lw.good());
}

TEST(CSVLineWriterTest, WriteOneBlankLine) {
  std::ostringstream data;
  csvio::util::CSVLineWriter csv_lw(data);

  csv_lw.writeline("");
  EXPECT_EQ("", data.str());
  EXPECT_EQ(1u, csv_lw.lcount());
  EXPECT_EQ(true, csv_lw.good());
}

TEST(CSVLineWriterTest, WriteOneSampleCSVLine) {
  std::ostringstream data;
  csvio::util::CSVLineWriter csv_lw(data);

  csv_lw.writeline("1,1,1,1,1,1,1,1");
  EXPECT_EQ("1,1,1,1,1,1,1,1", data.str());
  EXPECT_EQ(1u, csv_lw.lcount());
  EXPECT_EQ(true, csv_lw.good());
}

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
