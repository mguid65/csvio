#!/bin/bash

cmake . ${1}
make

for test in $(ls bin/*); do $test; done
