bazel run //src/pso/main:summary --cxxopt='-O3' --copt='-O3' --cxxopt='-DNDEBUG' --copt='-DNDEBUG' |& tee -a summaries/$(date --iso-8601=seconds)-summary
