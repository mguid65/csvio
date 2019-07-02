#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <functional>

template <typename RowContainer>
struct SplitFunction {
  static RowContainer delimiter_split(std::string_view input, std::string_view delim) {
    RowContainer output;
    size_t first = 0;
    while (first < input.size()) {
      const auto second = input.find_first_of(delim, first);
      if (first != second) output.emplace_back(input.substr(first, second - first));
      if (second == std::string_view::npos) break;
      first = second + 1;
    }
    return output;
  }
};

using VectorSplit = SplitFunction<std::vector<std::string_view>>;

template <typename RowContainer=std::vector<std::string_view>>
class CSVReader {
public:

  CSVReader() {}
  CSVReader(
      const std::string& filename, const std::string& delimiter = ",",
      std::ios_base::openmode flags = std::ios::in,
      std::function<RowContainer(std::string_view, std::string_view)>
          split_func = VectorSplit::delimiter_split)
      : m_infile(filename, flags),
        m_filename(filename),
        m_delim(delimiter),
        m_split_func(split_func) {}

  ~CSVReader() {
    m_infile.close();
  }
  void set_delimiter(std::string_view sv) { m_delim = sv; }
  std::string_view get_delimiter() { return m_delim; }
  const std::string_view get_delimiter() const { return m_delim; }

  bool good() { return m_infile.good(); }

  RowContainer& next() {
    advance();
    return m_current;
  }

private:
  void advance() {
    std::getline(m_infile, m_current_str_line);
    parse_current_str();
  }

  void parse_current_str() { m_current = m_split_func(m_current_str_line, m_delim); }
  std::ifstream m_infile;
  std::string_view m_filename;
  std::string_view m_delim;

  std::function<RowContainer(std::string_view, std::string_view)> m_split_func;
  std::string m_current_str_line;
  RowContainer m_current;
};

template <typename RowContainer=std::vector<std::string_view>>
class CSVWriter {
public:
  CSVWriter() {}
  CSVWriter(
      const std::string& filename, const std::string& delimiter=",",
      std::ios_base::openmode flags=std::ios::out)
      : m_outfile(filename, flags),
        m_filename(filename),
        m_delim(delimiter) {}

  ~CSVWriter() {
    m_outfile.close();
  }
  void set_delimiter(std::string_view sv) { m_delim = sv; }
  std::string_view get_delimiter() { return m_delim; }
  const std::string_view get_delimiter() const { return m_delim; }

  bool good() { return m_outfile.good(); }

  void write_header(const RowContainer& header) {
    if (header.empty()) return;
    bool first=true;
    for (auto & s: header) {
      if(!first) {
        m_outfile << m_delim << s;
      } else {
        m_outfile << s;
        first = false;
      }
    }
    m_outfile << '\n';
  }

  void write_row() {
    if (m_current.empty()) return;
    bool first=true;
    for (auto& s: m_current) {
      if(!first) {
        m_outfile << m_delim << s;
      } else {
        m_outfile << s;
        first = false;
      }
    }
    m_outfile << '\n';
  }

  void write_row(const RowContainer& values) {
    if (values.empty()) return;
    bool first = true;
    for (auto & s: values) {
      if(!first) {
        m_outfile << m_delim << s;
      } else {
        m_outfile << s;
        first = false;
      }
    }
    m_outfile << '\n';
  }

  void push_back(std::string_view sv) {
    m_current.push_back(sv);
  }

  void next_row() {
    m_current.clear();
  }

private:
  std::ofstream m_outfile;
  std::string_view m_filename;
  std::string_view m_delim;

  RowContainer m_current;
};
