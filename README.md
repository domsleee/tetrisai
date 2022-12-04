## USAGE

## Usage method 1 - natively
You need to install [bazel](https://bazel.build/install) and most likely `tbb` to get it running

See `cpp/myscript.sh`:
```
bazel run //src/pso/main:summary --cxxopt='-O3' --cxxopt='-DNDEBUG' --copt='-O3' --copt='-DNDEBUG'`
```

## Usage method 2 - using docker (doesn't work with M1 mac)
```
docker run --rm -v "$PWD":/usr/src/myapp -w /usr/src/myapp -it domslee/bazel:tbb22 bash myscript.sh
```

The docker images are maintained at [domsleee/bazel-docker](https://github.com/domsleee?tab=repositories&q=bazel&type=&language=&sort=)

## Structure
* `cpp/` contains the logic that evaluates the weights of feature sets
    - `MoveEvaluatorGroups.cpp` contains the feature sets that can be chosen
* `python/` contains the PSO logic used to generate the models
* `get-move-service/` an endpoint that is used by `web/` to play a game using the ai
* `web/` contains a way of running the generated models online


## Limitations

* The demo playback uses an emulator, meaning it can only tap the same key once every two frames (especially a problem for level 19, where the piece moves down every two frames)
* Rotations where the piece is off the top of the board aren't included in path finding
* The implementation is a mess
