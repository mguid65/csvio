#!/bin/bash

cmake . ${1}
make

if [ ! -f data/CSV_READER_BENCHMARK_001.csv ]; then
  if [ ! -d data ]; then
    mkdir data
  fi
  for i in $(seq 1 4000000); do
    printf "sometext,sometext,sometext,sometext\r\n" >> data/CSV_READER_BENCHMARK_001.csv;
  done
fi

for test in $(ls bin/*); do $test; done
