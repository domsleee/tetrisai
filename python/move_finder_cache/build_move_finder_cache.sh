#!/usr/bin/env bash
bazel build //src:move_finder_cache --cxxopt="-DMOVE_FINDER_CACHE"