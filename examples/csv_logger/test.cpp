#include "logger.hpp"
#include <iostream>

int main () {
  util::Logger log(std::cout);

  log.debug() << "Hello World!";
  log.info() << "Hello World!";
  log.warn() << "Hello World!";
  log.error() << "Hello World!";
  log.fatal() << "Hello World!";
}
