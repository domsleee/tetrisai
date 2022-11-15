## USAGE

## Usage method 1 - natively
You need to install [bazel](https://bazel.build/install) and most likely `tbb` to get it running

See `cpp/myscript.sh`:
```
bazel run //src/pso/main:summary --cxxopt='-O3' --cxxopt='-DNDEBUG' --copt='-O3' --copt='-DNDEBUG'`
```

## Usage method 2 - using docker (doesn't work with M1)
```
docker run --rm -v "$PWD":/usr/src/myapp -w /usr/src/myapp -it domslee/bazel:tbb2 bash myscript.sh
```

## Structure
* `cpp/` contains the logic that evaluates the weights of feature sets
    - `MoveEvaluatorGroups.cpp` contains the feature sets that can be chosen
* `python/` contains the PSO logic used to generate the models
* `web/` contains a way of running the generated models online