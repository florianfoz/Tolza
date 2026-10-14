#!/usr/bin/env bash

clear

set -e

cmake -S . -B build/debug -DTOLZA_TESTS=ON

cmake --build build/debug

ctest --test-dir build/debug --output-on-failure -V
