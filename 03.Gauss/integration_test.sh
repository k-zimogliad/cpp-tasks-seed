#!/bin/bash
set -euo pipefail

pwd
ls -la

./gauss AB.csv > output.csv

diff -u expected_output.csv output.csv