#!/usr/bin/env python3
# Rebuild and copy binary over to external_bin folder

import subprocess
import os
import shutil

# todo: replace with current path of script
DIR = "/Users/dom/Documents/git/tetrisAI/.docker/get-move-service/src"

def main():
  subprocess.check_call(["bazel", "build", "//src:ew_get_moves"])
  bazel_bin_path = get_bazel_bin_path()
  my_bin = os.path.join(bazel_bin_path, "src", "ew_get_moves")
  out_bin = os.path.join(DIR, "external_bin", "ew_get_moves")
  shutil.copy(my_bin, out_bin)

def get_bazel_bin_path():
  pr = subprocess.run(["bazel", "info"], stdout=subprocess.PIPE)
  my_out = pr.stdout.decode("utf-8")
  bazel_bin = my_out.split("\n")[0]
  path = bazel_bin.split(" ")[1].strip()
  return path

if __name__ == '__main__':
  main()
