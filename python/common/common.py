import os
import numpy

DIR = os.path.dirname(os.path.realpath(__file__))
DEFAULT_BINARY = os.path.join(DIR, '..', '..', 'bazel-bin', 'src', 'pso', 'main', 'ew')
EW_BENCH_BINARY = os.path.join(DIR, '..', '..', 'bazel-bin', 'src', 'pso', 'main', 'ew_bench')
MOVE_FINDER_CACHE_BINARY = os.path.join(DIR, '..', '..', 'bazel-bin', 'src', 'ew_move_finder_cache')

Particle = numpy.ndarray
