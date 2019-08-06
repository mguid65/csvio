/** \class MyLineReader
 *  \brief Derivation on the default line reader that terminates lines at character 0x01 instead of \r\n
 */
class MyLineReader {
public:
  /** \brief Construct a CSVLineReader from a reference to a std::istream
   *  \param instream reference to a std::istream
   */
  explicit MyLineReader(std::istream& instream) : m_csv_stream(instream) {
    m_result.reserve(1024);
  }

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
      if (m_state == LINE && (pos[0] == '\0' || pos[0] == EOF)) {
        *(++pos) = '\0';
        m_result.append(buf);
        pos = buf;
        break;
      } else if (m_state == LINE && pos[0] == '\"') {
        m_state = QUOTE;
      } else if (m_state == QUOTE && pos[0] == '\"') {
        m_state = LINE;
      } else if (m_state == QUOTE && !m_csv_stream.good()) {
        m_result = "";
        return m_result;
      }
      pos++;

      if (pos >= buf_end) {
        *(pos++) = '\0';
        m_result.append(buf);
        pos = buf;
      }
    }
    if (pos != buf) {
      *(pos++) = '\0';
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

