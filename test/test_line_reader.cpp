#include <sstream>

#include "csv_io.hpp"
#include "gtest/gtest.h"

namespace {

TEST(CSVLineReader, ConstructorFromIStream) {
  std::istringstream data("");
  CSVLineReader csv_lr(data);

  EXPECT_EQ(0u, csv_lr.lcount());
  EXPECT_EQ(true, csv_lr.good());
  EXPECT_EQ("", csv_lr.readline());
}

TEST(CSVLineReader, ReadOneBlankLine) {
  std::istringstream data("");
  CSVLineReader csv_lr(data);

  EXPECT_EQ("", csv_lr.readline());
  EXPECT_EQ(1u, csv_lr.lcount());
  EXPECT_EQ(false, csv_lr.good());
}

TEST(CSVLineReader, ReadOneSampleCSVLine) {
  std::istringstream data("1,1,1,1,1,1,1,1\n");
  CSVLineReader csv_lr(data);

  EXPECT_EQ("1,1,1,1,1,1,1,1\n", csv_lr.readline());
  EXPECT_EQ(1u, csv_lr.lcount());
  EXPECT_EQ(true, csv_lr.good());
}

TEST(CSVLineReader, CheckGood) {
  std::istringstream data("1,1,1,1,1,1,1,1\n");
  CSVLineReader csv_lr(data);

  EXPECT_EQ("1,1,1,1,1,1,1,1\n", csv_lr.readline());
  EXPECT_EQ("", csv_lr.readline());
  EXPECT_EQ(2u, csv_lr.lcount()); // not sure if I should let this be 1 or 2
  EXPECT_EQ(false, csv_lr.good());
}

TEST(CSVLineReader, ReadTwoSampleCSVLines) {
  std::istringstream data("1,1,1,1,1,1,1,1\n"
                          "2,2,2,2,2,2,2,2\n");
  CSVLineReader csv_lr(data);

  EXPECT_EQ("1,1,1,1,1,1,1,1\n", csv_lr.readline());
  EXPECT_EQ("2,2,2,2,2,2,2,2\n", csv_lr.readline());
  EXPECT_EQ(2u, csv_lr.lcount());
  EXPECT_EQ(true, csv_lr.good());
}

TEST(CSVLineReader, ReadOneSampleCSVLineAllNewLines) {
  std::istringstream data("\"1\n\",\"1\n\",\"1\n\",\"1\n\",\"1\n\",\"1\n\",\"1\n\",\"1\n\"\n");
  CSVLineReader csv_lr(data);

  EXPECT_EQ("\"1\n\",\"1\n\",\"1\n\",\"1\n\",\"1\n\",\"1\n\",\"1\n\",\"1\n\"\n", csv_lr.readline());
  EXPECT_EQ(1u, csv_lr.lcount());
  EXPECT_EQ(true, csv_lr.good());
}

TEST(CSVLineReader, ReadOneSampleLineHardParse) {
  std::istringstream data("\"\"\"one\"\"\",\"tw\n"
                          "o\",\"\"\"th,r\n"
                          "ee\"\"\",\"\"\"fo\n"
                          "u\"\"r\"\"\",5,6,7,8\n");

  CSVLineReader csv_lr(data);

  EXPECT_EQ("\"\"\"one\"\"\",\"tw\n"
            "o\",\"\"\"th,r\n"
            "ee\"\"\",\"\"\"fo\n"
            "u\"\"r\"\"\",5,6,7,8\n", csv_lr.readline());
  EXPECT_EQ("", csv_lr.readline());
  EXPECT_EQ(2u, csv_lr.lcount());
  EXPECT_EQ(false, csv_lr.good());
}

TEST(CSVLineReader, ReadOnePrematureEOF) {
  std::istringstream data("1,1,1,\"1\n");
  CSVLineReader csv_lr(data);

  EXPECT_EQ("", csv_lr.readline());
  EXPECT_EQ(1u, csv_lr.lcount());
  EXPECT_EQ(false, csv_lr.good());
}

} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
