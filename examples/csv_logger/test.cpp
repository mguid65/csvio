#include "logger.hpp"
#include <iostream>

void function_3(util::Logger& log) {
  log.Debug() << "Hello 3!";
}

void function_2(util::Logger& log) {
  log.Debug() << "Hello 2!";
  function_3(log);
}


void function_1(util::Logger& log) {
  log.Debug() << "Hello 1!";
  function_2(log);
}


int main () {
//  std::ofstream log_file("log_test.csv");
//  util::Logger log(log_file);
  util::Logger log(std::cout);


/*
  for (int i = 0; i < 1000000; i++) {
    log.Debug()() << "Hello World!";
  }
*/

  log.Debug() << "Hello World!";

  function_1(log);
}
