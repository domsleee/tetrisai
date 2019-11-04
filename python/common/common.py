import os
import numpy

DIR = os.path.dirname(os.path.realpath(__file__))
DEFAULT_BINARY = os.path.join(DIR, '..', '..', 'bazel-bin', 'src', 'ew')

Particle = numpy.ndarray
