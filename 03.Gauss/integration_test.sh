#!/bin/bash

./gauss AB.csv > output.csv || exit 1

if diff -u expected_output.csv output.csv; then
    echo "Integration test: PASSED"
    rm -f output.csv
    exit 0
else
    echo "Integration test: FAILED"
    exit 1
fi