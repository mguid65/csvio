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

#include <algorithm>
#include <cassert>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <istream>
#include <list>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#ifdef __GNUC__
#define likely(expr) (__builtin_expect(!!(expr), 1))
#define unlikely(expr) (__builtin_expect(!!(expr), 0))
#endif

namespace csvio::exceptional {
/** \class ColumnMismatchException
 *  \brief exception to describe mismatch in number of csv columns
 */
class ColumnMismatchException : public std::exception {
public:
  ColumnMismatchException() {}
  virtual const char* what() const noexcept { return "CSV column number mismatch detected"; }
};

}  // namespace csvio::exceptional

namespace csvio::util {
/** \class has_push_back
 *  \brief SFINAE Helper to determine if T has a push_back method
 */
template <typename, typename T>
struct has_push_back {
  static_assert(
      std::integral_constant<T, false>::value,
      "Second template parameter needs to be of function type.");
};

/** \class has_push_back
 *  \brief Specialization of has_push_back
 */
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
 *  \param delim used to check if a delimiter is in the field. if so, it should be escaped
 *  \param force_escape wrap the string in quotes even if not needed
 *  \return escaped csv field if necessary, otherwise the string
 */
inline std::string escape(std::string_view data, char delim = ',', bool force_escape = false) {
  std::string result;
  result.reserve(
      data.length());  // avoid lots of allocations by setting a reasonable initial allocation

  bool needs_escape{false};
  for (const char& c : data) {
    switch (c) {
      case '\"':
        result.push_back(c);
      case '\r':
      case '\n':
        needs_escape = true;
        break;
      default:
        if (c == delim) needs_escape = true;  // special handle for alternative delimiters
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

  if (data[0] == '\"') {  // if the first char is a quote, then the string is assumed to be quoted
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

enum CSVParserScope { LINE, QUOTE };

/** \class CSVInputParser
 *  \brief Class of functions to parse a csv row into a non map container
 *
 *  These functions assume the container has a push_back method
 */
template <template <class...> class RowContainer>
struct CSVInputParser {
  /** \brief Split string_view csv row based on a delimiter
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \return RowContainer of split csv fields
   */
  static RowContainer<std::string> delim_split_naive(const std::string& input, const char delim) {
    static_assert(
        has_push_back<
            RowContainer<std::string>,
            void(const typename RowContainer<std::string>::value_type&)>::value,
        "The template parameter needs to be a container type with an push_back function.");
    RowContainer<std::string> output;

    size_t first = 0;
    while (first < input.size()) {
      const auto second = input.find_first_of(delim, first);
      if (first != second) output.push_back(input.substr(first, second - first));
      if (second == std::string_view::npos) break;
      first = second + 1;
    }
    if (output.empty()) output.push_back("");
    return output;
  }

  /** \brief Split string_view csv row based on a delimiter with escaped fields
   *
   *  TODO(mguid65) Please Refactor
   *
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \return RowContainer of split csv fields, if input string empty, return RowContainer with one
   * empty string
   */
  static RowContainer<std::string> delim_split_escaped(std::string_view input, const char delim) {
    static_assert(
        has_push_back<
            RowContainer<std::string>,
            void(const typename RowContainer<std::string>::value_type&)>::value,
        "The template parameter needs to be a container type with an push_back function.");
    RowContainer<std::string> output;

    CSVParserScope state{LINE};
    char buf[256]{0};
    char* buf_end = buf + 255;
    char* pos = buf;

    std::string chunk;
    int num_cols{0};
    for (const auto& c : input) {
      switch (state) {
        case LINE:
          switch (c) {
            case '\"':
              state = QUOTE;
            default:
              if (c == delim || c == '\n') {
                if (c == delim) num_cols++;
                chunk.append(buf);

                output.push_back(chunk);

                chunk.clear();
                std::fill(buf, buf + 256, '\0');
                pos = buf;
              } else {
                pos[0] = c;
                pos++;
              }
          }
          break;
        case QUOTE:
          switch (c) {
            case '\"':
              state = LINE;
            default:
              pos[0] = c;
              pos++;
          }
          break;
      }
      if (pos > buf_end) {
        chunk.append(buf);
        std::fill(buf, buf + 256, '\0');
        pos = buf;
      }
    }

    chunk.append(buf);
    if (!chunk.empty() || output.size() == num_cols) output.push_back(chunk);

    auto& last_element = output.back();
    if (last_element.back() == '\r') {  // remove trailing carriage return
      last_element.pop_back();
    }

    return output;
  }

  /** \brief split a csv row on a delimiter escaping the output strings
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \return RowContainer of split csv fields, if input string empty, return RowContainer with one
   * empty string
   */
  static RowContainer<std::string> delim_split_unescaped(std::string_view input, const char delim) {
    RowContainer<std::string> result = delim_split_escaped(input, delim);
    for (auto& field : result) field = unescape(field);
    return result;
  }
};

/** \class CSVOutputFormatter
 *  \brief a class describing how csv output should be formatted
 */
template <template <class...> class RowContainer = std::vector>
struct CSVOutputFormatter {
  /** \brief Output formatter which joins the elements in a container
   *  with a delimiter while escaping fields
   *
   *  \param csv_row generic container of csv row values
   *  \param delim delimiter to delimit the output csv string fields
   *  \return delimited, escaped csv row
   */
  static std::string delim_join_escaped_fmt(
      const RowContainer<std::string>& csv_row, const char delim, const std::string& line_terminator) {
    std::string result;
    result.reserve(csv_row.size() * 2);

    bool first{true};
    for (auto& s : csv_row) {
#ifdef __GNUC__
      if (unlikely(first)) {
#else
      if (first) {
#endif
        result.append(escape(s));
        first = false;
      } else {
        result.push_back(delim);
        result.append(escape(s));
      }
    }
    result.append(line_terminator);
    return result;
  }

  /** \brief Output formatter which joins the elements in a container
   *  with a delimiter
   *
   *  \param csv_row generic container of csv row values
   *  \param delim delimiter to delimit the output csv string fields
   *  \return delimited  csv row
   */
  static std::string delim_join_unescaped_fmt(
      const RowContainer<std::string>& csv_row, const char delim, const std::string& line_terminator) {
    std::string result;
    result.reserve(csv_row.size() * 2);

    bool first{true};
    for (auto& s : csv_row) {
#ifdef __GNUC__
      if (unlikely(first)) {
#else
      if (first) {
#endif
        result.append(s);
        first = false;
      } else {
        result.push_back(delim);
        result.append(s);
      }
    }
    result.append(line_terminator);
    return result;
  }
};

/** \class CSVLineReader
 *  \brief Stateful CSV line reader which reads csv lines with escaped fields according to RFC 4180
 */
class CSVLineReader {
public:
  /** \brief Construct a CSVLineReader from a reference to a std::istream
   *  \param instream reference to a std::istream
   */
  CSVLineReader(std::istream& instream) : m_csv_stream(instream) {}

  /** \brief read a csv line
   *  \return a string with the contents of the csv line
   */
  std::string readline() {
    m_result.clear();

    char buf[1024]{0};
    char* buf_end = buf + 1023;
    char* pos = buf;

    while (m_csv_stream.good()) {
      m_csv_stream.get(*pos);
      if (m_state == LINE && pos[0] == '\n' || pos[0] == EOF) {
        m_result.append(buf);
        pos = buf;
        std::fill(buf, buf + 1024, '\0');
        break;
      } else if (m_state == LINE && pos[0] == '\"') {
        m_state = QUOTE;
      } else if (m_state == QUOTE && pos[0] == '\"') {
        m_state = LINE;
      } else if (m_state == QUOTE && !m_csv_stream.good()) {
        std::cerr << "Unexpected EOF" << '\n';  // maybe change to do something else
        m_result = "";
        return m_result;
      }
      pos++;

      if (pos >= buf_end) {
        pos = buf;
        m_result.append(buf);
        std::fill(buf, buf + 1024, '\0');
      }
    }
    if (pos != buf) {
      m_result.append(buf);
    }
    m_lines_read++;
    m_state = LINE;
    return m_result;
  }

  /** \brief Get number of csv lines read so far
   *  \return number of csv lines read so far
   */
  const size_t lcount() const { return m_lines_read; }

  /** \brief check if the underlying stream is still good
   *  \return true if good, otherwise false
   */
  bool good() { return m_csv_stream.good(); }

private:
  CSVParserScope m_state{LINE};
  std::istream& m_csv_stream;
  std::string m_result;
  size_t m_lines_read{0};
};

/** \class CSVLineWriter
 *  \brief Writes a csv line to a stream, provides a written lines counter
 *
 *  doesnt do much other than make the implementation symmetrical
 *  also allows for ever greater custom behavior,
 *  maybe some kind of custom format that prepends or appends some string before writing
 */
class CSVLineWriter {
public:
  /** \brief Construct a CSVLineWriter from an outstream
   *  \param outstream ostream to write lines to
   */
  CSVLineWriter(std::ostream& outstream) : m_csv_stream(outstream) {}

  /** \brief write a csv line to the stream
   *  \param line line to write to stream
   */
  void writeline(std::string_view line) {
    if (good()) {
      m_csv_stream.write(line.data(), line.length());
      m_lines_written++;
    }
  }

  /** \brief check if the underlying stream is still good
   *  \return true if good, otherwise false
   */
  bool good() { return m_csv_stream.good(); }

  /** \brief Get number of csv lines written so far
   *  \return number of csv lines written so far
   */
  const size_t lcount() const { return m_lines_written; }

private:
  std::ostream& m_csv_stream;
  size_t m_lines_written{0};
};

}  // namespace csvio::util

namespace csvio {

/** \class CSVReader
 *  \brief Reader to read a stream as CSV
 */
template <
    template <class...> class RowContainer = std::vector,
    typename LineReader = csvio::util::CSVLineReader>
class CSVReader {
public:
  /** \brief Construct a CSVReader from a reference to a generic LineReader
   *  \param line_reader reference to a LineReader
   *  \param delimiter a character delimiter
   *  \param has_header specify that this csv has a header
   *  \param strict_columns specify that an exception should be thrown in case of a column length
   * mismatch \param parse_func a function which describes how to split a csv row
   */
  CSVReader(
      LineReader& line_reader, const char delimiter = ',', bool has_header = false,
      bool strict_columns = true,
      std::function<RowContainer<std::string>(std::string_view, const char)> parse_func =
          csvio::util::CSVInputParser<RowContainer>::delim_split_unescaped)
      : m_csv_line_reader(line_reader),
        m_delim(delimiter),
        m_has_header(has_header),
        m_strict_columns(strict_columns),
        m_parse_func(parse_func) {
    if (m_has_header) {
      handle_header();
    }
  }

  /** \brief destroy a CSVReader
   *
   *  Does nothing special currently
   */
  ~CSVReader() {}

  /** \brief set the delimiter to a different character
   *  \param delim new delimiter
   */
  void set_delimiter(const char delim) { m_delim = delim; }

  /** \brief get the current delimiter
   *  \return constant char delimiter value
   */
  const char get_delimiter() const { return m_delim; }

  /** \brief check if the underlying stream is still good
   *  \return true if good, otherwise false
   */
  bool good() { return m_csv_line_reader.good(); }

  /** \brief Get the csv headers if they exist
   *  \return a reference to the RowContainer of headers, otherwise a RowContainer with one blank
   * element
   */
  RowContainer<std::string>& get_header_names() { return m_header_names; }

  /** \brief Return the current RowContainer of CSV values
   *  \return a reference to the current RowContainer
   */
  RowContainer<std::string>& current() { return m_current; }

  /** \brief Advance to the next row and return the current RowContainer of CSV values
   *  \return a reference to the current RowContainer
   */
  RowContainer<std::string>& read() {
    try {
      advance();
      return current();
    } catch (csvio::exceptional::ColumnMismatchException& e) {
      throw;
    }
  }

private:
  /** \brief advance the current string and parse it
   */
  void advance() {
    m_current_str_line = m_csv_line_reader.readline();
    try {
      parse_current_str();
    } catch (csvio::exceptional::ColumnMismatchException& e) {
      throw;
    }
  }

  /** \brief parse the current string line to a RowContainer
   */
  void parse_current_str() {
    if (m_current_str_line.empty()) {
      m_current.clear();
      m_current.push_back("");
      return;
    }
    m_current = m_parse_func(m_current_str_line, m_delim);

    if (m_num_columns == -1) {
      m_num_columns = m_current.size();
    } else if (m_strict_columns && (m_current.size() != m_num_columns)) {
      throw csvio::exceptional::ColumnMismatchException();
    }
  }

  /** \brief handle reading and parsing the header if has_header is true
   */
  void handle_header() {
    m_current_str_line = m_csv_line_reader.readline();
    m_header_names = m_parse_func(m_current_str_line, m_delim);
    m_num_columns = m_header_names.size();
    m_current_str_line = "";
  }

  LineReader& m_csv_line_reader;

  char m_delim;
  bool m_has_header;
  bool m_strict_columns;
  std::function<RowContainer<std::string>(std::string_view, const char)> m_parse_func;

  std::string m_current_str_line;
  long m_num_columns{-1};

  RowContainer<std::string> m_header_names{""};
  RowContainer<std::string> m_current{""};
};

/** \class CSVWriter
 *  \brief Writes data as csv
 */
template <
    template <class...> class RowContainer = std::vector,
    class LineWriter = csvio::util::CSVLineWriter>
class CSVWriter {
public:
  /** \brief construct a CSVWriter from a LineWriter
   *  \param line_writer reference to a LineWriter object
   *  \param delimiter output delimiter to use to delimit ouput
   *  \param strict_columns whether same length columns should be enforced
   *  \param line_terminator sequence that denotes the end of a csv row
   *  \param format_func a function to format a container to an output csv string
   */
  CSVWriter(
      LineWriter& line_writer, const char delimiter = ',', bool strict_columns = true, const std::string& line_terminator = "\r\n",
      std::function<std::string(const RowContainer<std::string>&, const char, const std::string&)> format_func =
          csvio::util::CSVOutputFormatter<RowContainer>::delim_join_escaped_fmt)
      : m_csv_line_writer(line_writer),
        m_delim(delimiter),
        m_strict_columns(strict_columns),
        m_line_terminator(line_terminator),
        m_csv_output_formatter(format_func) {}

  /** \brief set a new delimiter for this writer
   *  \param delim new delimiter
   */
  void set_delimiter(const char delim) { m_delim = delim; }

  /** \brief get the current delimiter from this writer
   *  \return constant character delimiter
   */
  const char get_delimiter() const { return m_delim; }

  /** \brief check if the underlying stream is still good
   *  \return true if good, otherwise false
   */
  bool good() { return m_csv_line_writer.good(); }

  /** \brief write the csv header, sets number of columns
   *  \param header RowContainer of string header names
   */
  void write_header(const RowContainer<std::string>& header) {
    if (header.empty()) return;
    m_num_columns = header.size();
    m_csv_line_writer.writeline(m_csv_output_formatter(header, m_delim, m_line_terminator));
  }

  /** \brief write a csv row, may set initial number of columns
   *  \param header RowContainer of string values
   */
  void write(const RowContainer<std::string>& values) {
    if (values.empty()) return;
    if (m_num_columns == -1) {
      m_num_columns = values.size();
    } else if (m_strict_columns && (values.size() != m_num_columns)) {
      throw csvio::exceptional::ColumnMismatchException();
    }
    m_csv_line_writer.writeline(m_csv_output_formatter(values, m_delim, m_line_terminator));
  }

private:
  char m_delim;

  bool m_strict_columns;
  long m_num_columns{-1};
  std::string m_line_terminator;

  std::function<std::string(const RowContainer<std::string>&, const char, const std::string&)> m_csv_output_formatter;

  LineWriter& m_csv_line_writer;
};

}  // namespace csvio
