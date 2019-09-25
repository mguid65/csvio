#ifndef _CSV_IO_HPP_
#define _CSV_IO_HPP_

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
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace csvio {

#if defined(__cpp_lib_string_view) || defined(__cpp_lib_experimental_string_view)
#include <string_view>
using std::string_view;
#else
using string_view = const std::string&;
#endif

using std::function;
using std::istream;
using std::ostream;
using std::string;
using std::thread;
using std::vector;
using std::map;

#ifdef __GNUC__
#define likely(expr) (__builtin_expect(!!(expr), 1))
#define unlikely(expr) (__builtin_expect(!!(expr), 0))
#endif

/** \internal */
namespace util {

/** \brief function to escape characters in csv fields according to RFC 4180
 *  \ingroup utility
 *
 *  Source: https://tools.ietf.org/html/rfc4180
 *
 *  If discovered that the string doesn't have any characters which need to be escaped,
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
inline string escape(string_view data, char delim = ',', bool force_escape = false) {
  string result;
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
 *  \ingroup utility
 *
 *  Generally linear avg time and space complexity
 *
 *  \param data string_view csv data to unescape
 *  \return unquoted csv field
 */
inline string unescape(string_view data) {
  string result;
  int quotes_seen = 0;

#if defined(__cpp_lib_string_view) || defined(__cpp_lib_experimental_string_view)
  if (data[0] == '\"') {  // if the first char is a quote, then the string is assumed to be quoted
    data = data.substr(1, data.size() - 2);
  }
  string_view sv_data = data;
#else
  // keep const string& (which is "string_view" now) construction, and if statement.
  string_view sv_data = (data[0] == '\"') ? data.substr(1, data.size() - 2) : data;
#endif

  for (const char& c : sv_data) {
    if (c == '\"') {
      quotes_seen++;
      if (quotes_seen == 2) {
        quotes_seen = 0;
        result.push_back('\"');
      }
    } else {
      quotes_seen = 0;
      result.push_back(c);
    }
  }
  return result;
}

enum CSVParserScope { LINE, QUOTE };


/** \class DelimSplitNaive
 *  \ingroup parser
 *  \brief Split a CSV assuming no escaped characters
 */
template <template <class...> class RowContainer>
struct DelimSplitNaive {
  /** \brief Split string_view csv row based on a delimiter
   *
   *  Linear time complexity with respect to the size of input
   *
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \return RowContainer of split csv fields
   */
  RowContainer<string>& operator() (string_view input, const char delim) {
    m_data.clear();

    string tmp(input.data(), input.length());

    size_t first = 0;
    while (first < tmp.size()) {
      const auto second = tmp.find_first_of(delim, first);
      if (first != second) m_data.push_back(tmp.substr(first, second - first));
      if (second == string::npos) break;
      first = second + 1;
    }
    if (m_data.empty()) m_data.push_back("");
    return m_data;
  }

  RowContainer<string> m_data;
};


/** \class DelimSplitEscaped
 *  \ingroup parser
 *  \brief Split a CSV assuming it is escaped and leave it escaped
 */
template <template <class...> class RowContainer>
struct DelimSplitEscaped {
  /** \brief Split string_view csv row based on a delimiter with escaped fields
   *
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \return RowContainer of split csv fields, if input string empty, return RowContainer with one
   * empty string
   */
  RowContainer<string>& operator()(string_view input, const char delim) {
    m_data.clear();
    delim_split_escaped_impl(input, delim);
    return m_data;
  }

  /** \brief implementation of delim_split_escaped
   *
   *  Linear time complexity with respect to the size of input
   *
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \return RowContainer of split csv fields, if input string empty, return RowContainer with one
   * empty string
   */
  void delim_split_escaped_impl(string_view input, const char delim) {
    CSVParserScope state{LINE};

    string chunk;
    chunk.reserve(256);
    int num_cols{1};

    for (const auto& c : input) {
      if (state == LINE) {
        if (c == '\"') {
          state = QUOTE;
          chunk.push_back(c);
        } else {
          if (c == delim || c == '\n') {
            if (c == delim) num_cols++;
            m_data.push_back(chunk);
            chunk.clear();
          } else {
            chunk.push_back(c);
          }
        }
      } else {
        if (c == '\"') state = LINE;
        chunk.push_back(c);
      }
    }
    if (!chunk.empty() || m_data.size() < num_cols) m_data.push_back(chunk);

    auto& last_element = m_data.back();
    if (last_element.back() == '\r') last_element.pop_back();
  }

  RowContainer<string> m_data;
};


/** \class DelimSplitUnescapedThreaded
 *  \ingroup parser
 *  \brief Split a CSV assuming it is escaped and unescape the output
 *
 *  Threaded unescaping for output
 */
template <template <class...> class RowContainer>
struct DelimSplitUnescapedThreaded {
  /** \brief split a csv row on a delimiter escaping the output strings threaded
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \return RowContainer of split csv fields, if input string empty, return RowContainer with one
   * empty string
   */
  RowContainer<string>& operator()(string_view input, const char delim) {
    m_data.clear();
    delim_split_unescaped_threaded_impl(input, delim);
    return m_data;
  }

  /** \brief implementation of delim_split_escaped
   *
   *  Linear time complexity with respect to the size of input
   *
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \return RowContainer of split csv fields, if input string empty, return RowContainer with one
   * empty string
   */
  void delim_split_escaped_impl(string_view input, const char delim) {
    CSVParserScope state{LINE};

    string chunk;
    chunk.reserve(256);
    int num_cols{1};

    for (const auto& c : input) {
      if (state == LINE) {
        if (c == '\"') {
          state = QUOTE;
          chunk.push_back(c);
        } else {
          if (c == delim || c == '\n') {
            if (c == delim) num_cols++;
            m_data.push_back(chunk);
            chunk.clear();
          } else {
            chunk.push_back(c);
          }
        }
      } else {
        if (c == '\"') state = LINE;
        chunk.push_back(c);
      }
    }
    if (!chunk.empty() || m_data.size() < num_cols) m_data.push_back(chunk);

    auto& last_element = m_data.back();
    if (last_element.back() == '\r') last_element.pop_back();
  }

  /** \brief implementation of delim_split_unescaped_threaded
   *
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \return RowContainer of split csv fields, if input string empty, return RowContainer with one
   * empty string
   */
  void delim_split_unescaped_threaded_impl(string_view input, const char delim) {
    delim_split_escaped_impl(input, delim);
    vector<thread> threads;  // would be better to use a thread pool

    for (auto& field : m_data) {
      threads.emplace_back([&]() { field = unescape(field); });
    }

    for (auto& worker : threads) {
      if (worker.joinable()) worker.join();
    }
  }

  RowContainer<string> m_data;
};


/** \class DelimSplitUnescaped
 *  \ingroup parser
 *  \brief Split a CSV assuming it is escaped and unescape the output
 */
template <template <class...> class RowContainer>
struct DelimSplitUnescaped {
  /** \brief split a csv row on a delimiter unescaping the output strings threaded
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \return RowContainer of split csv fields, if input string empty, return RowContainer with one
   * empty string
   */
  RowContainer<string>& operator()(string_view input, const char delim) {
    m_data.clear();
    delim_split_unescaped_impl(input, delim);
    return m_data;
  }

  /** \brief implementation of delim_split_escaped
   *
   *  Linear time complexity with respect to the size of input
   *
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \return RowContainer of split csv fields, if input string empty, return RowContainer with one
   * empty string
   */
  void delim_split_escaped_impl(string_view input, const char delim) {
    CSVParserScope state{LINE};

    string chunk;
    chunk.reserve(256);
    int num_cols{1};

    for (const auto& c : input) {
      if (state == LINE) {
        if (c == '\"') {
          state = QUOTE;
          chunk.push_back(c);
        } else {
          if (c == delim || c == '\n') {
            if (c == delim) num_cols++;
            m_data.push_back(chunk);
            chunk.clear();
          } else {
            chunk.push_back(c);
          }
        }
      } else {
        if (c == '\"') state = LINE;
        chunk.push_back(c);
      }
    }
    if (!chunk.empty() || m_data.size() < num_cols) m_data.push_back(chunk);

    auto& last_element = m_data.back();
    if (last_element.back() == '\r') last_element.pop_back();
  }

  /** \brief implementation of delim_split_unescaped
   *
   *  O(2N) where n is the size of the input
   *
   *  \param input string_view to split by a delimiter
   *  \param delim delimiter to split on
   *  \return RowContainer of split csv fields, if input string empty, return RowContainer with one
   * empty string
   */
  void delim_split_unescaped_impl(string_view input, const char delim) {
    delim_split_escaped_impl(input, delim);
    for (auto& field : m_data) field = unescape(field);
  }

  RowContainer<string> m_data;
};

/** \class MapDelimSplitUnescaped
 *  \ingroup parser
 *  \brief Split a CSV to a map assuming it is escaped and unescape the output
 */
template <template <class...> class RowMapContainer>
struct MapDelimSplitUnescaped {

  /** \brief split a csv row on a delimiter unescaping the output strings
   *  \param input string_view to split by a delimiter
   *  \param delim delimiter to split input on
   *  \param header_names input canonical header names
   *  \return RowMapContainer of split csv fields
   */
  RowMapContainer<string, string>& operator() (
      string_view input, const char delim, vector<string>& header_names) {
    m_data.clear();
    delim_split_unescaped_impl(input, delim, header_names);
    return m_data;
  }

  /** \brief implementation of delim_split_escaped for map types
   *
   *  Linear time complexity with respect to the size of input
   *
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \param header_names input canonical header names
   */
  void delim_split_escaped_impl(
      string_view input, const char delim, vector<string>& header_names) {
    CSVParserScope state{LINE};

    string chunk;
    chunk.reserve(256);
    int num_cols{1};

    auto current_header = header_names.begin();
    string last_header;
    for (const auto& c : input) {
      if (state == LINE) {
        if (c == '\"') {
          state = QUOTE;
          chunk.push_back(c);
        } else {
          if (c == delim || c == '\n') {
            if (c == delim) num_cols++;
            m_data.emplace(*current_header, chunk);
            ++current_header;
            chunk.clear();
          } else {
            chunk.push_back(c);
          }
        }
      } else {
        if (c == '\"') state = LINE;
        chunk.push_back(c);
      }
      if (current_header != header_names.end()) last_header = *current_header;
    }
    if (!chunk.empty() || m_data.size() < num_cols) m_data.emplace(*current_header, chunk);

    if (auto& last_element = m_data.at(last_header); last_element.back() == '\r')
      last_element.pop_back();
  }


  /** \brief split and unescape the fields in the csv
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \param header_names input canonical header names
   */
  void delim_split_unescaped_impl(
      string_view input, const char delim, vector<string>& header_names) {
    delim_split_escaped_impl(input, delim, header_names);
    for (auto& key_field : m_data) key_field.second = unescape(key_field.second);
  }

  RowMapContainer<string, string> m_data;
};


/** \class MapDelimSplitUnescapedThreaded
 *  \ingroup parser
 *  \brief Split a CSV to a map container assuming the values are escaped, and unescape output
 */
template <template <class...> class RowMapContainer>
struct MapDelimSplitUnescapedThreaded {
  /** \brief split a csv row on a delimiter unescaping the output strings
   *  \param input string_view to split by a delimiter
   *  \param delim delimiter to split input on
   *  \param header_names input canonical header names
   *  \return RowMapContainer of split csv fields
   */
  RowMapContainer<string, string>& operator() (
      string_view input, const char delim, vector<string>& header_names) {
    m_data.clear();
    delim_split_unescaped_threaded_impl(input, delim, header_names);
    return m_data;
  }

  /** \brief implementation of delim_split_escaped for map types
   *
   *  Linear time complexity with respect to the size of input
   *
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \param header_names input canonical header names
   */
  void delim_split_escaped_impl(
      string_view input, const char delim, vector<string>& header_names) {
    CSVParserScope state{LINE};

    string chunk;
    chunk.reserve(256);
    int num_cols{1};

    auto current_header = header_names.begin();
    string last_header;
    for (const auto& c : input) {
      if (state == LINE) {
        if (c == '\"') {
          state = QUOTE;
          chunk.push_back(c);
        } else {
          if (c == delim || c == '\n') {
            if (c == delim) num_cols++;
            m_data.emplace(*current_header, chunk);
            ++current_header;
            chunk.clear();
          } else {
            chunk.push_back(c);
          }
        }
      } else {
        if (c == '\"') state = LINE;
        chunk.push_back(c);
      }
      if (current_header != header_names.end()) last_header = *current_header;
    }
    if (!chunk.empty() || m_data.size() < num_cols) m_data.emplace(*current_header, chunk);

    if (auto& last_element = m_data.at(last_header); last_element.back() == '\r')
      last_element.pop_back();
  }


  /** \brief split and unescape the fields in the csv with a thread for each value
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \param header_names input canonical header names
   */
  void delim_split_unescaped_threaded_impl(
      string_view input, const char delim, vector<string>& header_names) {
    delim_split_escaped_impl(input, delim, header_names);
    std::vector<std::thread> threads;  // would be better to use a thread pool

    for (auto& key_field : m_data) {
      threads.emplace_back([&]() { key_field.second = unescape(key_field.second); });
    }

    for (auto& worker : threads) {
      if (worker.joinable()) worker.join();
    }
  }

  RowMapContainer<string, string> m_data;
};


/** \class MapDelimSplitEscaped
 *  \ingroup parser
 *  \brief Split a CSV to a map assuming values are escaped and leave them escaped
 */
template <template <class...> class RowMapContainer>
struct MapDelimSplitEscaped {
  /** \brief split a csv row on a delimiter
   *  \param input string_view to split by a delimiter
   *  \param delim delimiter to split input on
   *  \param header_names input canonical header names
   *  \return RowMapContainer of split csv fields
   */
  RowMapContainer<string, string>& operator() (string_view input, const char delim, vector<string>& header_names) {
    m_data.clear();
    delim_split_escaped_impl(input, delim, header_names);
    return m_data;
  }

  /** \brief implementation of delim_split_escaped for map types
   *
   *  Linear time complexity with respect to the size of input
   *
   *  \param input string_view to split by delimiter
   *  \param delim delimiter to split on
   *  \param header_names input canonical header names
   */
  void delim_split_escaped_impl(
      string_view input, const char delim, vector<string>& header_names) {
    CSVParserScope state{LINE};

    string chunk;
    chunk.reserve(256);
    int num_cols{1};

    auto current_header = header_names.begin();
    string last_header;
    for (const auto& c : input) {
      if (state == LINE) {
        if (c == '\"') {
          state = QUOTE;
          chunk.push_back(c);
        } else {
          if (c == delim || c == '\n') {
            if (c == delim) num_cols++;
            m_data.emplace(*current_header, chunk);
            ++current_header;
            chunk.clear();
          } else {
            chunk.push_back(c);
          }
        }
      } else {
        if (c == '\"') state = LINE;
        chunk.push_back(c);
      }
      if (current_header != header_names.end()) last_header = *current_header;
    }
    if (!chunk.empty() || m_data.size() < num_cols) m_data.emplace(*current_header, chunk);

    if (auto& last_element = m_data.at(last_header); last_element.back() == '\r')
      last_element.pop_back();
  }

  RowMapContainer<string, string> m_data;
};


/** \class DelimJoinEscapedFormat
 *  \ingroup formatter
 *  \brief Join a CSV on a delimiter escaping each field
 */
template <template <class...> class RowContainer = vector>
struct DelimJoinEscapedFormat {

  /** \brief get an escaped csv string from a vector of csv values
   *  \param csv_row csv row to join to a csv string
   *  \param delim delimiter to join values on
   *  \param line_terminator line terminating sequence
   *  \return an escaped csv string of the joined vector
   */
  string& operator() (
      const RowContainer<string>& csv_row, const char delim, const string& line_terminator) {
    m_data.clear();
    m_data.reserve(csv_row.size() * 2);

    bool first{true};
    for (auto& s : csv_row) {
#ifdef __GNUC__
      if (unlikely(first)) {
#else
      if (first) {
#endif
        m_data.append(escape(s));
        first = false;
      } else {
        m_data.push_back(delim);
        m_data.append(escape(s));
      }
    }
    m_data.append(line_terminator);
    return m_data;
  }

  string m_data;
};


/** \class DelimJoinUnescapedFormat
 *  \ingroup formatter
 *  \brief Join a CSV on a delimiter leaving each field unescaped
 */
template <template <class...> class RowContainer = vector>
struct DelimJoinUnescapedFormat {

  /** \brief get an unescaped csv string from a vector of csv values
   *  \param csv_row csv row to join to a csv string
   *  \param delim delimiter to join values on
   *  \param line_terminator line terminating sequence
   *  \return an unescaped csv string of the joined vector
   */
  string& operator() (
      const RowContainer<string>& csv_row, const char delim, const string& line_terminator) {
    m_data.clear();
    bool first{true};
    for (auto& s : csv_row) {
#ifdef __GNUC__
      if (unlikely(first)) {
#else
      if (first) {
#endif
        m_data.append(s);
        first = false;
      } else {
        m_data.push_back(delim);
        m_data.append(s);
      }
    }
    m_data.append(line_terminator);
    return m_data;
  }

  string m_data;
};


/** \class CSVLineReader
 *  \ingroup line_reader
 *  \brief Stateful CSV line reader which reads csv lines with escaped fields according to RFC 4180
 */
class CSVLineReader {
public:
  /** \brief Construct a CSVLineReader from a reference to a istream
   *  \param instream reference to a istream
   */
  explicit CSVLineReader(istream& instream) : m_csv_stream(instream) {}

  /** \brief read a csv line
   *  \return a string with the contents of the csv line
   */
  string readline() {
    m_result.clear();

    std::string buf;
    buf.reserve(1024);

    char c;

    while (m_csv_stream.good()) {
      m_csv_stream.get(c);
      if (m_state == LINE && (c == '\n' || c == EOF || !m_csv_stream.good())) {
        if (c == '\n') m_result.push_back(c);
        break;
      } else if (m_state == LINE && c == '\"') {
        m_state = QUOTE;
      } else if (m_state == QUOTE && c == '\"') {
        m_state = LINE;
      } else if (m_state == QUOTE && !m_csv_stream.good()) {
        m_result = "";
        return m_result;
      }
      m_result.push_back(c);
    }

    // handle the possibility of the last line, we dont want to parse it
    // we will inspect the next three characters and see if eofbit is set.
    // if it is not, then we will unget these characters;
    m_csv_stream.get();
    m_csv_stream.get();
    m_csv_stream.get();

    if (good()) {
      m_csv_stream.unget();
      m_csv_stream.unget();
      m_csv_stream.unget();
    }

    m_lines_read++;
    m_state = LINE;
    return m_result;
  }

  /** \brief Get number of csv lines read so far
   *  \return number of csv lines read so far
   */
  [[nodiscard]] size_t lcount() const { return m_lines_read; }

  /** \brief check if the underlying stream is still good
   *  \return true if good, otherwise false
   */
  bool good() { return m_csv_stream.good(); }

private:
  CSVParserScope m_state{LINE};
  istream& m_csv_stream;
  string m_result;
  size_t m_lines_read{0};
};

/** \class CSVSimpleLineReader
 *  \ingroup line_reader
 *  \brief CSV line reader which reads csv lines
 *
 *  This line reader is a sane alternative to the conforming line reader which
 *  allows for newline characters in your csv fields.
 *  If you have an insane dataset which allows newlines in the middle of fields,
 *  I'm sorry...
 *
 *  This should be a slightly faster alternative for most cases.
 */
class CSVSimpleLineReader {
public:
  /** \brief Construct a CSVSimpleLineReader from a reference to a istream
   *  \param instream reference to a istream
   */
  explicit CSVSimpleLineReader(std::istream& instream) : m_csv_stream(instream) {}

  /** \brief read a csv line
   *  \return a string with the contents of the csv line
   */
  std::string readline() {
    m_result.clear();

    std::getline(m_csv_stream, m_result);

    // handle the possibility of the last line, we dont want to parse it
    // we will inspect the next three characters and see if eofbit is set.
    // if it is not, then we will unget these characters;
    m_csv_stream.get();
    m_csv_stream.get();
    m_csv_stream.get();

    if (good()) {
      m_csv_stream.unget();
      m_csv_stream.unget();
      m_csv_stream.unget();
    }

    m_lines_read++;
    return m_result;
  }

  /** \brief Get number of csv lines read so far
   *  \return number of csv lines read so far
   */
  [[nodiscard]] size_t lcount() const { return m_lines_read; }

  /** \brief check if the underlying stream is still good
   *  \return true if good, otherwise false
   */
  bool good() { return m_csv_stream.good(); }

private:
  std::istream& m_csv_stream;
  std::string m_result;
  size_t m_lines_read{0};
};

/** \class CSVLineWriter
 *  \ingroup line_writer
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
  explicit CSVLineWriter(ostream& outstream) : m_csv_stream(outstream) {}

  /** \brief write a csv line to the stream
   *  \param line line to write to stream
   */
  void writeline(string_view line) {
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
  [[nodiscard]] size_t lcount() const { return m_lines_written; }

private:
  ostream& m_csv_stream;
  size_t m_lines_written{0};
};

}  // namespace util

/** \class CSVReader
 *  \ingroup reader
 *  \brief Reader to read a stream as CSV
 */
template <
    template <class...> class RowContainer = vector,
    typename LineReader = csvio::util::CSVLineReader,
    typename Parser = csvio::util::DelimSplitUnescaped<RowContainer> >
class CSVReader {
public:
  /** \class iterator
   *  \brief CSVReader iterator
   */
  struct iterator {
    /** \brief construct a CSVReader iterator
     *  This default is used for the end iterator
     */
    iterator() { m_good = false; }

    /** \brief parameterized constructor that takes a pointer to a CSVReader
     *  \param ptr pointer to a valid non-nullptr CSVReader iterator
     */
    explicit iterator(CSVReader* ptr) : m_ptr(ptr) {
      m_ptr->read();
      m_good = m_ptr->good();
    }

    /** \brief increment this CSVReader iterator
     *  \return the iteration of CSVReader
     */
    iterator operator++() {
      m_ptr->read();
      m_good = m_ptr->good();
      return *this;
    }

    /** \brief Comparison to check for end
     *  \param other const ref to a CSVReader iterator
     *  \return true if the iterator is still good; otherwise false
     */
    bool operator!=(const iterator& other) const { return m_good != other.m_good; }

    /** \brief return the current RowContainer reference from this iterator
     *  \return a reference to the current RowContainer
     */
    RowContainer<string>& operator*() const { return m_ptr->current(); }

    /** \brief return the current RowContainer pointed to by this iterator
     *  \return a pointer to the current RowContainer
     */
    RowContainer<string>* operator->() const { return &m_ptr->current(); }

    CSVReader* m_ptr;
    bool m_good{true};
  };

  /** \brief the begin CSVReader iterator
   *  \return iterator with the begin CSVReader
   */
  iterator begin() { return iterator(this); }

  /** \brief the end CSVReader iterator
   *  \return default constructed end iterator
   */
  iterator end() const { return iterator(); }

  /** \brief Construct a CSVReader from a reference to a generic LineReader
   *  \param line_reader reference to a LineReader
   *  \param delimiter a character delimiter
   *  \param has_header specify that this csv has a header
   *  \param warn_columns warn in case of mismatched columns
   *  \param parse_func a function which describes how to split a csv row
   */
  explicit CSVReader(
      LineReader& line_reader, const char delimiter = ',', bool has_header = false,
      bool warn_columns = true)
      : m_csv_line_reader(line_reader),
        m_delim(delimiter),
        m_has_header(has_header),
        m_warn_columns(warn_columns) {
    if (m_has_header) {
      handle_header();
    }
  }

  /** \brief set the delimiter to a different character
   *  \param delim new delimiter
   */
  void set_delimiter(const char delim) { m_delim = delim; }

  /** \brief get the current delimiter
   *  \return constant char delimiter value
   */
  [[nodiscard]] char get_delimiter() const { return m_delim; }

  /** \brief check if the underlying stream is still good
   *  \return true if good, otherwise false
   */
  bool good() { return m_csv_line_reader.good(); }

  /** \brief Get the csv headers if they exist
   *  \return a reference to the RowContainer of headers, otherwise a RowContainer with one blank
   * element
   */
  RowContainer<string>& get_header_names() { return m_header_names; }

  /** \brief Return the current RowContainer of CSV values
   *  \return a reference to the current RowContainer
   */
  RowContainer<string>& current() { return m_current; }

  /** \brief Advance to the next row and return the current RowContainer of CSV values
   *  \return a reference to the current RowContainer
   */
  RowContainer<string>& read() {
    advance();
    return current();
  }

  /** \brief Get number of csv lines read so far
   *  \return number of csv lines read so far
   */
  [[nodiscard]] size_t lcount() const { return m_csv_line_reader.lcount(); }

protected:
  /** \brief advance the current string and parse it
   */
  void advance() {
    m_current_str_line = m_csv_line_reader.readline();
    parse_current_str();
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
    } else if (m_warn_columns && (m_current.size() != m_num_columns)) {
      std::cerr << "[warning] Column mismatch detected, further parsing may be malformed\n";
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
  bool m_warn_columns;

  Parser m_parse_func;
  string m_current_str_line;
  long m_num_columns{-1};

  RowContainer<string> m_header_names{""};
  RowContainer<string> m_current{""};
};

/** \class CSVWriter
 *  \ingroup writer
 *  \brief Writes data as csv
 */
template <
    template <class...> class RowContainer = vector, class LineWriter = csvio::util::CSVLineWriter, class Formatter = csvio::util::DelimJoinEscapedFormat<RowContainer> >
class CSVWriter {
public:
  /** \brief construct a CSVWriter from a LineWriter
   *  \param line_writer reference to a LineWriter object
   *  \param delimiter output delimiter to use to delimit ouput
   *  \param warn_columns whether warning should be printed in the case of column mismatch
   *  \param line_terminator sequence that denotes the end of a csv row
   *  \param format_func a function to format a container to an output csv string
   */
  explicit CSVWriter(
      LineWriter& line_writer, const char delimiter = ',', bool warn_columns = true,
      string line_terminator = "\r\n" )
      : m_csv_line_writer(line_writer),
        m_delim(delimiter),
        m_warn_columns(warn_columns),
        m_line_terminator(std::move(line_terminator)) {}

  /** \brief set a new delimiter for this writer
   *  \param delim new delimiter
   */
  void set_delimiter(const char delim) { m_delim = delim; }

  /** \brief get the current delimiter from this writer
   *  \return constant character delimiter
   */
  [[nodiscard]] char get_delimiter() const { return m_delim; }

  /** \brief check if the underlying stream is still good
   *  \return true if good, otherwise false
   */
  bool good() { return m_csv_line_writer.good(); }

  /** \brief write the csv header, sets number of columns
   *  \param header RowContainer of string header names
   */
  void write_header(const RowContainer<string>& header) {
    if (header.empty()) return;
    m_num_columns = header.size();
    m_csv_line_writer.writeline(m_csv_output_formatter(header, m_delim, m_line_terminator));
  }

  /** \brief write a csv row, may set initial number of columns
   *  \param header RowContainer of string values
   */
  void write(const RowContainer<string>& values) {
    if (values.empty()) return;
    if (m_num_columns == -1) {
      m_num_columns = values.size();
    } else if (m_warn_columns && (values.size() != m_num_columns)) {
      std::cerr << "[Warning] Column mismatch detected\n";
    }
    m_csv_line_writer.writeline(m_csv_output_formatter(values, m_delim, m_line_terminator));
  }

  /** \brief Get number of csv lines written so far
   *  \return number of csv lines written so far
   */
  [[nodiscard]] size_t lcount() const { return m_csv_line_writer.lcount(); }

protected:
  char m_delim;

  bool m_warn_columns;
  long m_num_columns{-1};
  string m_line_terminator;

  Formatter m_csv_output_formatter;

  LineWriter& m_csv_line_writer;
};

/** \class CSVMapReader
 *  \ingroup reader
 *  \brief Reader to read a stream as CSV into a map like container
 */
template <
    template <class...> class RowMapContainer = std::map,
    class LineReader = csvio::util::CSVLineReader,
    class Parser = csvio::util::MapDelimSplitUnescaped<RowMapContainer>,
    class HeaderParser = csvio::util::DelimSplitUnescaped<std::vector>>
class CSVMapReader {
public:
  /** \brief Construct a CSVReader from a reference to a generic LineReader
   *  \param line_reader reference to a LineReader
   *  \param delimiter a character delimiter
   *  \param parse_func a function which describes how to split a csv row
   *  \param header_parse_func a function to parse the header
   *  because it doesn't go into the map (default is
   * csvio::util::CSVInputParser<std::vector>::delim_split_unescaped)
   */

  explicit CSVMapReader(
      LineReader& line_reader, const char delimiter = ',')
      : m_csv_line_reader(line_reader),
        m_delim(delimiter) {
    handle_header();
  }

  /** \brief set the delimiter to a different character
   *  \param delim new delimiter
   */
  void set_delimiter(const char delim) { m_delim = delim; }

  /** \brief get the current delimiter
   *  \return constant char delimiter value
   */
  [[nodiscard]]char get_delimiter() const { return m_delim; }

  /** \brief check if the underlying stream is still good
   *  \return true if good, otherwise false
   */
  bool good() { return m_csv_line_reader.good(); }

  /** \brief Return the current RowContainer of CSV values
   *  \return a reference to the current RowContainer
   */
  RowMapContainer<std::string, std::string>& current() { return m_current; }

  const auto& get_header_names() const { return m_stream_order_header_names; }
  /** \brief Advance to the next row and return the current RowContainer of CSV values
   *  \return a reference to the current RowContainer
   */
  RowMapContainer<std::string, std::string>& read() {
    advance();
    return current();
  }

  /** \brief Get number of csv lines read so far
   *  \return number of csv lines read so far
   */
  [[nodiscard]] size_t lcount() const { return m_csv_line_reader.lcount(); }

protected:
  /** \brief advance the current string and parse it
   */
  void advance() {
    m_current_str_line = m_csv_line_reader.readline();
    parse_current_str();
  }

  /** \brief parse the current string line to a RowContainer
   */
  void parse_current_str() {
    if (m_current_str_line.empty()) {
      m_current.clear();
      return;
    }
    m_current = m_parse_func(m_current_str_line, m_delim, m_stream_order_header_names);
  }

  /** \brief handle reading and parsing the header if has_header is true
   */
  void handle_header() {
    m_current_str_line = m_csv_line_reader.readline();
    m_stream_order_header_names = m_header_parse_func(m_current_str_line, m_delim);
    m_current_str_line.clear();
  }

  LineReader& m_csv_line_reader;

  char m_delim;

  Parser m_parse_func;
  HeaderParser m_header_parse_func;

  std::string m_current_str_line;

  // this will store header names in the canonical order from the source stream
  // in case a map type is used that does some arbitrary reordering
  std::vector<std::string> m_stream_order_header_names;
  RowMapContainer<std::string, std::string> m_current{};
};

} // namespace csvio

#endif  // CSV_IO_HPP
