#!/usr/bin/env python3
# Rebuild and copy binary over to external_bin folder

import subprocess
import os
import shutil
from pathlib import Path

DIR = Path(__file__).parent.absolute()
BAZEL_BIN_DIR = os.path.join(DIR, '..', '..', 'cpp', 'bazel-bin')


def main():
  bazel_bin_path = get_bazel_bin_path()
  subprocess.check_call(["bazel", "build", "//src/pso/main:ew_get_moves"], cwd=bazel_bin_path)
  my_bin = os.path.join(bazel_bin_path, "src", "pso", "main", "ew_get_moves")
  out_bin = os.path.join(DIR, "external_bin", "ew_get_moves")
  os.remove(out_bin)
  shutil.copy(my_bin, out_bin)

def get_bazel_bin_path():
  return BAZEL_BIN_DIR

if __name__ == '__main__':
  main()
