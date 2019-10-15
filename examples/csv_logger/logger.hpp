#include "csv_io.hpp"
#include <chrono>
#include <string>
#include <memory>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace util {

std::string DateTimeNow() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

enum LogLevel {
  DEBUG,
  INFO,
  WARN,
  ERROR,
  FATAL,
};

std::array<std::string, 5> level_to_string{ "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };

class Logger {
 public:
  struct Message : public std::ostringstream {
    Message(Logger& logger, LogLevel level) : m_logger(logger), m_level(level) {}
    ~Message() {
      m_logger.Log({DateTimeNow(), level_to_string[m_level], str()});
    }
   private:
     Logger& m_logger;
     LogLevel m_level;
  };

  explicit Logger(std::ostream& log_sink) {
    m_writer.reset(new csvio::util::CSVLineWriter(log_sink));
    m_csv_writer.reset(new csvio::CSVWriter(*m_writer));
    m_csv_writer->write_header({"TIME","LEVEL","MESSAGE"});
  }

  Message debug() { return { *this, LogLevel::DEBUG }; }
  Message info() {return { *this, LogLevel::INFO }; }
  Message warn() {return { *this, LogLevel::WARN }; }
  Message error() {return { *this, LogLevel::ERROR }; }
  Message fatal() {return { *this, LogLevel::FATAL }; }

  void Log(std::initializer_list<std::string> msg) {
    m_csv_writer->write(msg);
  }

 private:
  std::unique_ptr<csvio::util::CSVLineWriter> m_writer;
  std::unique_ptr<csvio::CSVWriter<std::vector>> m_csv_writer;
};

} // namespace util
