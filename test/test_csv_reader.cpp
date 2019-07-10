#include <string>
#include <sstream>
#include <vector>
#include "csv_io.hpp"
#include "gtest/gtest.h"

namespace {

TEST(CSVReaderTest, ConstructorFromLineReader) {
  std::istringstream instream("");
  CSVLineReader csv_lr(instream);
  CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{""};
  EXPECT_EQ(expected, csv_reader.current());
  EXPECT_EQ(true, csv_reader.good());

  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneCSVLine) {
  std::istringstream instream("a,b,c,d,e");
  CSVLineReader csv_lr(instream);
  CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLine) {
  std::istringstream instream("\"a\",\"b\",\"c\",\"d\",\"e\"");
  CSVLineReader csv_lr(instream);
  CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineWithEscapedQuotes) {
  std::istringstream instream("\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"");
  CSVLineReader csv_lr(instream);
  CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

