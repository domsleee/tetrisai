#!/usr/bin/env bash
set -e;
echo "${BASH_SOURCE[0]}"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )";
PARENT_DIR=$( cd ${DIR}/.. && pwd )

cd "${PARENT_DIR}"
bazel build //src/pso/main:bench_time --cxxopt='-O3' --cxxopt='-DNDEBUG' --copt='-O3' --copt='-DNDEBUG' --cxxopt='-pg' --copt='-pg' --linkopt='-pg' \
           --cxxopt='-fno-inline-functions' --cxxopt='-fno-inline-functions-called-once' --cxxopt='-fno-optimize-sibling-calls'
cd "${PARENT_DIR}/bazel-bin/src/pso/main"
./bench_time
gprof bench_time gmon.out > /tmp/analysis.txt
graph2dot.py -s /tmp/analysis.txt -w  | dot -Tpng -o "${PARENT_DIR}"/scripts/performance.png
graph2dot.py -s /tmp/analysis.txt -o "${PARENT_DIR}/scripts/hi.dot"