import os
import numpy

DIR = os.path.dirname(os.path.realpath(__file__))
BAZEL_BIN_DIR = os.path.join(DIR, '..', '..', 'cpp', 'bazel-bin')
DEFAULT_BINARY = os.path.join(BAZEL_BIN_DIR, 'src', 'pso', 'main', 'ew')
EW_BENCH_BINARY = os.path.join(BAZEL_BIN_DIR, 'src', 'pso', 'main', 'ew_bench')
MOVE_FINDER_CACHE_BINARY = os.path.join(BAZEL_BIN_DIR, 'src', 'ew_move_finder_cache')

Particle = numpy.ndarray
