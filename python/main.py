#!/usr/bin/env python
from tetrisai import runner
from tetrisai.runner_settings import RunnerSettings
from tetrisai.run_particle import RunParticle
from common.common import DEFAULT_BINARY
import argparse
import logging
import os
from tetrisai.performance_logger import PerformanceLogger


def main(args):
  run_particle = RunParticle(args.binary)
  runner_settings = RunnerSettings(dimensions=run_particle.get_num_dimensions(), num_particles=24*4, iterations=1200*3)
  run_particle.print_config()
  performance_logger = PerformanceLogger(run_particle, filename=args.binary.split('/')[-1] + '.log')
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

