#!/usr/bin/env bash
bazel query //... | xargs bazel test
