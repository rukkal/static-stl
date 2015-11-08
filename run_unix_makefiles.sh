#!/bin/bash

mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
if (($? == 0)); then
   ./etltest
fi

