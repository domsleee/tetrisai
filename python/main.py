#!/usr/bin/env python3
from tetrisai import runner
from tetrisai.runner_settings import RunnerSettings
from common.common import DEFAULT_BINARY
import argparse
import logging
import os

logging.basicConfig()

def main(args):
  runner_settings = RunnerSettings(num_particles=4)
  print(runner_settings)
  runner.MyRunner(args.binary, runner_settings).run()

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("--binary", help="binary used for PSO", default=DEFAULT_BINARY)
  args = parser.parse_args()
  main(args)

