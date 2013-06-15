#!/bin/bash
clang++ -D__APPLE__ -O3 -std=c++11 -Wno-non-template-friend -Wunused-value -o traverse $1
