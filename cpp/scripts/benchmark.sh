bazel run //src/pso/main:bench_time --cxxopt -O3 --cxxopt -DNDEBUG --copt -O3 --copt -DNDEBUG
#bazel run //src/pso/main:bench_time -c opt --cxxopt -O3 --cxxopt -DNDEBUG --copt -O3 --copt -DNDEBUG