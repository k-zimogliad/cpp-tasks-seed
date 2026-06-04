#!/bin/bash
set -euo pipefail

./gauss AB.csv > output.csv

diff -u expected_output.csv output.csv

echo "Integration test: PASSED"
rm -f output.csv