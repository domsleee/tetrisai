#!/usr/bin/env python
from tetrisai import runner
from tetrisai.runner_settings import RunnerSettings
from tetrisai.run_particle import RunParticle
from common.common import DEFAULT_BINARY
import argparse
import logging
import os
from tetrisai.performance_logger import PerformanceLogger

logging.basicConfig()

def main(args):
  runner_settings = RunnerSettings(dimensions=20)
  print(runner_settings)
  run_particle = RunParticle(args.binary)
  run_particle.print_config()
  performance_logger = PerformanceLogger(run_particle)
  performance_logger.log_settings(runner_settings)
  if args.note is not None:
    performance_logger.note(args.note)
  runner.MyRunner(runner_settings, run_particle, performance_logger=performance_logger).run()

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("--binary", help="binary used for PSO", default=DEFAULT_BINARY)
  parser.add_argument("--note", help="optional note")
  args = parser.parse_args()
  main(args)

