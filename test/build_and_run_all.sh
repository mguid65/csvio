#!/bin/bash

cmake . ${1} -DTEST_DEBUG_MODE=yes
make

for test in $(ls bin/*); do $test; done
