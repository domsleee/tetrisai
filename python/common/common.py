import os
import numpy

DIR = os.path.dirname(os.path.realpath(__file__))
PROJ_DIR = os.path.join(DIR, '..')
BAZEL_BIN_DIR = os.path.join(DIR, '..', '..', 'cpp', 'bazel-bin')
BAZEL_BINARY = os.path.join(BAZEL_BIN_DIR, "src", "pso", "main", "get_moves")
DEFAULT_BINARY = os.environ.get("DEFAULT_BINARY", BAZEL_BINARY)
BENCH_BINARY = os.path.join(BAZEL_BIN_DIR, 'src', 'pso', 'main', 'get_moves_bench')
MOVE_FINDER_CACHE_BINARY = os.path.join(BAZEL_BIN_DIR, 'src', 'move_finder_cache')

Particle = numpy.ndarray
