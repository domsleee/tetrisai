from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from src.pybind import math

assert math.add(1, 1) == 2
assert math.subtract(1, 1) == 0
print("ok")