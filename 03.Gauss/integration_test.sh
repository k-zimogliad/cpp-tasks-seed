#!/bin/bash

./gauss AB.csv > output.csv

if cmp -s output.csv expected_output.csv; then
    echo "Integration test: PASSED"
    rm -f output.csv
    exit 0
else
    echo "Integration test: FAILED"
    rm -f output.csv
    exit 1
fi