#include <algorithm>
#include <cassert>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <string_view>
#include <vector>

template <typename, typename T>
struct has_push_back {
  static_assert(
      std::integral_constant<T, false>::value,
      "Second template parameter needs to be of function type.");
};

template <typename C, typename Ret, typename... Args>
struct has_push_back<C, Ret(Args...)> {
private:
  template <typename T>
  static constexpr auto check(T*) -> typename std::is_same<
      decltype(std::declval<T>().push_back(std::declval<Args>()...)), Ret>::type;

  template <typename>
  static constexpr std::false_type check(...);

  typedef decltype(check<C>(0)) type;

public:
  static constexpr bool value = type::value;
};

namespace csvio::util {
inline std::string escape(std::string_view data) {
  std::string result;
  bool needs_escape{false};
  for (const char& c : data) {
    switch (c) {
      case '\"':
        result.push_back(c);
      case '\n':
      case ',':
        needs_escape = true;
    }
    result.push_back(c);
  }

  if (needs_escape) {
    result.push_back('\"');
    result.insert(0, "\"");
  }
  return result;
}

inline std::string unescape(std::string_view data) {
  std::string result;
  int quotes_seen = 0;
  if (data[0] == '\"') {
    data = data.substr(1, data.size() - 2);
  }

  for (const char& c : data) {
    switch (c) {
      case '\"':
        quotes_seen++;
        if (quotes_seen == 2) {
          quotes_seen = 0;
          result.push_back('\"');
        }
        break;
      default:
        quotes_seen = 0;
        result.push_back(c);
    }
  }
  return result;
}

}  // namespace csvio::util

template <typename RowContainer>
struct SplitFunction {
  static RowContainer delimiter_split(std::string_view input, std::string_view delim) {
    static_assert(
        has_push_back<RowContainer, void(const typename RowContainer::value_type&)>::value,
        "The template parameter needs to be a container type with an push_back function.");
    RowContainer output;
    size_t first = 0;
    while (first < input.size()) {
      const auto second = input.find_first_of(delim, first);
      if (first != second) output.push_back(input.substr(first, second - first));
      if (second == std::string_view::npos) break;
      first = second + 1;
    }
    return output;
  }
};

using VectorSplitSV = SplitFunction<std::vector<std::string_view>>;
using VectorSplitS = SplitFunction<std::vector<std::string>>;
using ListSplitSV = SplitFunction<std::list<std::string_view>>;
using ListSplitS = SplitFunction<std::list<std::string>>;

class ColumnMismatchException : public std::exception {
  virtual const char* what() const throw() { return "CSV column number mismatch detected"; }
} ColumnMismatchException;

template <typename RowContainer = std::vector<std::string_view>>
class CSVReader {
public:
  CSVReader() {}
  CSVReader(
      const std::string& filename, const std::string& delimiter = ",",
      std::ios_base::openmode flags = std::ios::in,
      std::function<RowContainer(std::string_view, std::string_view)> split_func =
          VectorSplitSV::delimiter_split,
      bool has_header = false, bool strict_columns = true)
      : m_infile(filename, flags),
        m_filename(filename),
        m_delim(delimiter),
        m_split_func(split_func),
        m_has_header(has_header),
        m_strict_columns(strict_columns) {}

  ~CSVReader() { m_infile.close(); }
  void set_delimiter(std::string_view sv) { m_delim = sv; }
  std::string_view get_delimiter() { return m_delim; }
  const std::string_view get_delimiter() const { return m_delim; }

  bool good() { return m_infile.good(); }

  RowContainer& current() { return m_current; }

  RowContainer& read() {
    advance();
    return current();
  }

private:
  void advance() {
    std::getline(m_infile, m_current_str_line);
    parse_current_str();
  }

  void parse_current_str() {
    if (m_current_str_line.empty()) {
      m_current.clear();
      return;
    }
    m_current = m_split_func(m_current_str_line, m_delim);
    if (m_num_columns == -1) {
      m_num_columns = m_current.size();
    } else if (m_strict_columns && (m_num_columns != m_current.size())) {
      throw ColumnMismatchException;
    }
  }

  void handle_header() {
    m_header_names = m_split_func(m_current_str_line, m_delim);
    m_num_columns = m_header_names.size();
  }

  std::ifstream m_infile;
  std::string_view m_filename;
  std::string_view m_delim;

  std::function<RowContainer(std::string_view, std::string_view)> m_split_func;
  bool m_has_header;
  bool m_strict_columns;

  std::string m_current_str_line;
  long m_num_columns{-1};

  RowContainer m_header_names;
  RowContainer m_current;
};

template <typename RowContainer = std::vector<std::string_view>>
class CSVWriter {
public:
  CSVWriter() {}
  CSVWriter(
      const std::string& filename, const std::string& delimiter = ",",
      std::ios_base::openmode flags = std::ios::out, bool strict_columns = true)
      : m_outfile(filename, flags),
        m_filename(filename),
        m_delim(delimiter),
        m_strict_columns(strict_columns) {}

  ~CSVWriter() { m_outfile.close(); }
  void set_delimiter(std::string_view sv) { m_delim = sv; }
  std::string_view get_delimiter() { return m_delim; }
  const std::string_view get_delimiter() const { return m_delim; }

  bool good() { return m_outfile.good(); }

  void write_header(const RowContainer& header) {
    if (header.empty()) return;
    m_num_columns = header.size();
    bool first = true;
    for (auto& s : header) {
      if (!first) {
        m_outfile << m_delim << s;
      } else {
        m_outfile << s;
        first = false;
      }
    }
    m_outfile << "\r\n";
  }

  void write_row() {
    if (m_current.empty()) return;
    if (m_num_columns == -1) {
      m_num_columns = m_current.size();
    } else if (m_strict_columns && (m_current.size() != m_num_columns)) {
      throw ColumnMismatchException;
    }
    bool first = true;
    for (auto& s : m_current) {
      if (!first) {
        m_outfile << m_delim << s;
      } else {
        m_outfile << s;
        first = false;
      }
    }
    m_outfile << "\r\n";
  }

  void write_row(const RowContainer& values) {
    if (values.empty()) return;
    if (m_num_columns == -1) {
      m_num_columns = values.size();
    } else if (m_strict_columns && (values.size() != m_num_columns)) {
      throw ColumnMismatchException;
    }
    bool first = true;
    for (auto& s : values) {
      if (!first) {
        m_outfile << m_delim << s;
      } else {
        m_outfile << s;
        first = false;
      }
    }
    m_outfile << "\r\n";
  }

  void write_raw_csv_row(std::string_view raw_csv) { m_outfile << raw_csv << "\r\n"; }

  void push_back(std::string_view sv) { m_current.push_back(sv); }

  void next_row() { m_current.clear(); }

private:
  std::ofstream m_outfile;
  std::string_view m_filename;
  std::string_view m_delim;

  bool m_strict_columns;
  long m_num_columns{-1};

  RowContainer m_current;
};
