#include <sstream>
#include <string>
#include <vector>
#include "csv_io.hpp"
#include "gtest/gtest.h"

namespace {

TEST(CSVReaderTest, ConstructorFromLineReader) {
  std::istringstream instream("");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{""};
  EXPECT_EQ(expected, csv_reader.current());
  EXPECT_EQ(true, csv_reader.good());

  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadCSVHeader) {
  std::istringstream instream("a,b,c,d,e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr, ',', true);

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.get_header_names());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneCSVLine) {
  std::istringstream instream("a,b,c,d,e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ColumnMismatchExceptionTest) {
  std::istringstream instream(
      "a,b,c,d,e\n"
      "a,b,c,d\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_ANY_THROW(csv_reader.read());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLine) {
  std::istringstream instream("\"a\",\"b\",\"c\",\"d\",\"e\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineWithEscapedQuotes) {
  std::istringstream instream(
      "\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr);

  std::vector<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneCSVLineAltDelim) {
  std::istringstream instream("a|b|c|d|e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr, '|');

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineAltDelim) {
  std::istringstream instream("\"a\"|\"b\"|\"c\"|\"d\"|\"e\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr, '|');

  std::vector<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltDelim) {
  std::istringstream instream(
      "\"\"\"a\"\"\"|\"\"\"b\"\"\"|\"\"\"c\"\"\"|\"\"\"d\"\"\"|\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader csv_reader(csv_lr, '|');

  std::vector<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ConstructorFromLineReaderAltContainer) {
  std::istringstream instream("");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr);

  std::list<std::string> expected{""};
  EXPECT_EQ(expected, csv_reader.current());
  EXPECT_EQ(true, csv_reader.good());

  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadCSVHeaderAltContainer) {
  std::istringstream instream("a,b,c,d,e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr, ',', true);

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.get_header_names());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneCSVLineAltContainer) {
  std::istringstream instream("a,b,c,d,e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr);

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ColumnMismatchExceptionTestAltContainer) {
  std::istringstream instream(
      "a,b,c,d,e\n"
      "a,b,c,d\n");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr);

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_ANY_THROW(csv_reader.read());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineAltContainer) {
  std::istringstream instream("\"a\",\"b\",\"c\",\"d\",\"e\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr);

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltContainer) {
  std::istringstream instream(
      "\"\"\"a\"\"\",\"\"\"b\"\"\",\"\"\"c\"\"\",\"\"\"d\"\"\",\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr);

  std::list<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneCSVLineAltDelimAltContainer) {
  std::istringstream instream("a|b|c|d|e");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr, '|');

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineAltDelimAltContainer) {
  std::istringstream instream("\"a\"|\"b\"|\"c\"|\"d\"|\"e\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr, '|');

  std::list<std::string> expected{"a", "b", "c", "d", "e"};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

TEST(CSVReaderTest, ReadOneEscapedCSVLineWithEscapedQuotesAltDelimAltContainer) {
  std::istringstream instream(
      "\"\"\"a\"\"\"|\"\"\"b\"\"\"|\"\"\"c\"\"\"|\"\"\"d\"\"\"|\"\"\"e\"\"\"");
  csvio::util::CSVLineReader csv_lr(instream);
  csvio::CSVReader<std::list> csv_reader(csv_lr, '|');

  std::list<std::string> expected{"\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\""};
  EXPECT_EQ(expected, csv_reader.read());
  EXPECT_EQ(false, csv_reader.good());
}

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
