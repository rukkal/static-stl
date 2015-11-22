#!/bin/bash
mkdir build
mkdir build/unix_makefiles
cd build/unix_makefiles
cmake -G "Unix Makefiles" ../..
make
if (($? == 0)); then
   ./test
fi
