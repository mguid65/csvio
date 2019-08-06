# Custom Line Reader

Custom line readers should be of the form:

```
/** \class LineReader
 *  \brief A generic LineReader definition
 */
class LineReader {
 public:
  /** \brief Construct a LineReaderBase
   */
  LineReader(...) {...}

  /** \brief Read what you define to be a line
   *  \return a std::string of the line without the terminating character
   */
  std::string readline() {...}

  /** \brief Get the number of valid lines read by the reader
   *  \return Number of valid lines read
   */
  const size_t lcount() const {...}

  /** \brief Check whether the stream is still good
   *  \return true if good; otherwise false
   */
  bool good() {...}
 private:
  ...
};
```

