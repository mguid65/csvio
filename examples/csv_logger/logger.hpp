#include <chrono>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include "csv_io.hpp"

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

std::array<std::string, 5> level_to_string{"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

class Logger {
public:
  struct Message : public std::ostringstream {
    Message(Logger& logger, LogLevel level, const std::string& location = "")
        : m_logger(logger), m_level(level), m_location(location) {}
    ~Message() { m_logger.Log({DateTimeNow(), level_to_string[m_level], m_location, str()}); }

  private:
    Logger& m_logger;
    LogLevel m_level;
    std::string m_location;
  };

  explicit Logger(std::ostream& log_sink) {
    m_writer.reset(new csvio::util::CSVLineWriter(log_sink));
    m_csv_writer.reset(new csvio::CSVWriter(*m_writer));
    m_csv_writer->write_header({"TIME", "LEVEL", "FUNC", "MESSAGE"});
  }

  Message debug_(std::string func = "") { return {*this, LogLevel::DEBUG, func}; }
  Message info_(std::string func = "") { return {*this, LogLevel::INFO, func}; }
  Message warn_(std::string func = "") { return {*this, LogLevel::WARN, func}; }
  Message error_(std::string func = "") { return {*this, LogLevel::ERROR, func}; }
  Message fatal_(std::string func = "") { return {*this, LogLevel::FATAL, func}; }

#define Debug() debug_(__func__)
#define Info() info_(__func__)
#define Warn() warn_(__func__)
#define Error() error_(__func__)
#define Fatal() fatal_(__func__)

  void Log(const std::initializer_list<std::string>& msg) { m_csv_writer->write(msg); }

private:
  std::unique_ptr<csvio::util::CSVLineWriter> m_writer;
  std::unique_ptr<csvio::CSVWriter<std::vector>> m_csv_writer;
};

}  // namespace util
