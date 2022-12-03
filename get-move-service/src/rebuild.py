#!/usr/bin/env python3
# Rebuild and copy binary over to external_bin folder

import subprocess
import os
import shutil
from pathlib import Path

DIR = Path(__file__).parent.absolute()
WORKSPACE_DIR = os.path.join(DIR, '..', '..', 'cpp')


def main():
  os.chdir(WORKSPACE_DIR)
  print(WORKSPACE_DIR)
  subprocess.check_call(["bazel", "build", "//src/pso/main:get_moves", "--cxxopt", "-O3", "--cxxopt", "-DNDEBUG", "--copt", "-O3", "--copt", "-DNDEBUG"], cwd=WORKSPACE_DIR)
  #my_bin = os.path.join(WORKSPACE_DIR, "bazel-bin", "src", "pso", "main", "get_moves")
  #out_bin = os.path.join(DIR, "external_bin", "get_moves")
  #try:
  #  os.remove(out_bin)
  #except Exception as e:
  #  pass
  #shutil.copy(my_bin, out_bin)

if __name__ == '__main__':
  main()
