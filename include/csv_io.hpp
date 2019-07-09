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


/** \class has_push_back
 *  \brief SFINAE Helper to determine if T has a push_back method
 */
template <typename, typename T>
struct has_push_back {
  static_assert(
      std::integral_constant<T, false>::value,
      "Second template parameter needs to be of function type.");
};

template <typename C, typename Ret, typename... Args>
struct has_push_back<C, Ret(Args...)> {
private:

  /** \brief check if a type T has method push_back
   *  \return boolean whether T has a method push_back
   */
  template <typename T>
  static constexpr auto check(T*) -> typename std::is_same<
      decltype(std::declval<T>().push_back(std::declval<Args>()...)), Ret>::type;


  /** \brief overload which returns false
   *  \return false
   */
  template <typename>
  static constexpr std::false_type check(...);

  typedef decltype(check<C>(0)) type;

public:
  static constexpr bool value = type::value;
};

namespace csvio::util {

/** \brief function to escape characters in csv fields according to RFC 4180
 *
 *  Source: https://tools.ietf.org/html/rfc4180
 *
 *  If discoverd that the string doesn't have any characters which need to be escaped,
 *  the string will not be enclosed in quotes
 *
 *  Also handles alternative delimiters and allows forced escapes
 *  which will append leading and trailing double quotes
 *
 *  Generally O(n) avg time and space complexity
 *
 *  \param data string to escape
 *  \return escaped csv field if necessary, otherwise the string
 */
inline std::string escape(std::string_view data, char delim=',', bool force_escape=false) {
  std::string result;
  result.reserve(data.length()); // avoid lots of allocations by setting a reasonable initial allocation

  bool needs_escape{false};
  for (const char& c : data) {
    switch (c) {
      case '\"':
        result.push_back(c);
      case '\n':
        needs_escape = true;
        break;
      default:
        if (c == delim) needs_escape = true; // special handle for alternative delimiters
    }
    result.push_back(c);
  }

  if (needs_escape || force_escape) {
    result.push_back('\"');
    result.insert(0, "\"");
  }

  return result;
}


/** \brief unescape a csv escaped string according to RFC 4180
 *
 *  Generally O(n) avg time and space complexity
 *
 *  \param data string_view csv data to unescape
 *  \return unquoted csv field
 */
inline std::string unescape(std::string_view data) {
  std::string result;
  int quotes_seen = 0;

  if (data[0] == '\"') { // if the first char is a quote, then the string is assumed to be quoted
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


/** \class SplitFunction
 *  \brief 
 */
template <typename RowContainer>
struct SplitFunction {

  /** \brief 
   *  \param input
   *  \param delim
   *  \return
   */
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


  /** \brief 
   *  \param input
   *  \param delim
   *  \return
   */
  static RowContainer delimiter_esc_split(std::string_view input, std::string_view delim) {
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


/** \class ColumnMismatchException
 *  \brief exception to describe mismatch in number of csv columns
 */
class ColumnMismatchException : public std::exception {
  virtual const char* what() const throw() { return "CSV column number mismatch detected"; }
} ColumnMismatchException;

// maybe implement a class which gives the option of returning a raw line in strict conformance to RFC 4180 or
// in a mode where it only returns lines whether or not they are valid csv


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

class CSVLineReader {
 public:
  enum Scope {
    LINE,
    QUOTE
  };

  CSVLineReader(std::istream& instream) : m_csv_stream(instream) {}

  std::string readline() {
      m_result.clear();

      char buf[1024]{0};
      char * buf_end = buf + 1023;
      char * pos = buf;

      while(m_csv_stream.good()) {
        m_csv_stream.get(*pos);
        if (m_state == LINE && pos[0] == '\n') {
          m_result.append(buf);
          break;
        } else if (m_state == LINE && pos[0] == '\"') {
          m_state = QUOTE;
        } else if (m_state == QUOTE && pos[0] == '\"') {
          m_state = LINE;
        } else if (m_state == QUOTE && pos[0] == EOF) {
          std::cerr << "Unexpected EOF\n" << '\n'; // maybe change to do something else
          m_result = "";
          pos = buf;
          break;
        } else if (m_state == LINE && pos[0] == EOF) {
          m_result.append(buf);
          break;
        }
        pos++;

        if(pos >= buf_end) {
          pos = buf;
          m_result.append(buf);
          std::fill(buf, buf + 1024, '\0');
        }
      }
      m_lines_read++;
    return m_result;
  }

  const size_t lcount() const {
    return m_lines_read;
  }

  bool good() {
    return m_csv_stream.good();
  }

 private:
  Scope m_state{LINE};
  std::istream& m_csv_stream;
  std::string m_result;
  size_t m_lines_read{0};
};

