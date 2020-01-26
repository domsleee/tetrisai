#!/usr/bin/env python
from tetrisai import runner
from tetrisai.runner_settings import RunnerSettings
from tetrisai.run_particle import RunParticle
from common.common import DEFAULT_BINARY
import argparse
import logging
import os

logging.basicConfig()

def main(args):
  runner_settings = RunnerSettings()
  print(runner_settings)
  run_particle = RunParticle(args.binary)
  runner.MyRunner(runner_settings, run_particle).run()

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("--binary", help="binary used for PSO", default=DEFAULT_BINARY)
  args = parser.parse_args()
  main(args)

