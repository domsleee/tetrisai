set -e;
$(cd ../python; ./script.sh);
bazel run //src/pso/main:summary --cxxopt='-O3' --cxxopt='-DNDEBUG' --copt='-O3' --copt='-DNDEBUG'

