#!/usr/bin/env bash
bazel build //src:ew_move_finder_cache --cxxopt="-DMOVE_FINDER_CACHE"