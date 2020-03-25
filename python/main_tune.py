#!/usr/bin/env python
from tetrisai import runner
from tetrisai.runner_settings import RunnerSettings
from tetrisai.run_particle import RunParticle
from common.common import DEFAULT_BINARY
import argparse
import logging
import os
from tetrisai.performance_logger import PerformanceLogger
import timeit
import datetime


NUM_RUNS = 7
NUM_ITERATIONS = 450

def main(args):
  logging.basicConfig(level=logging.ERROR)
  estimate_time()
  result = {}
  pos = {}
  for nparticles in range(24, 24*NUM_RUNS + 1, 24):
    result[nparticles], pos[nparticles] = go(nparticles, NUM_ITERATIONS)
    print(nparticles, result[nparticles], pos[nparticles])
  sresult = {k: v for k, v in sorted(result.items(), key=lambda item: item[1])}
  print(sresult)

def estimate_time():
  timeForOneGroupOneIteration = timeit.timeit(lambda: go(24, 1), number=1)
  # 1, 12
  timeForNGroupsOneIteration = (NUM_RUNS * (1 + NUM_RUNS)) / 2
  print("N groups 1 iteration", timeForNGroupsOneIteration)
  timeForNGroupsNIterations = timeForNGroupsOneIteration * NUM_ITERATIONS
  print("estimated time")
  bonus = timeForNGroupsNIterations * 3.5
  print(datetime.timedelta(seconds=bonus))

def go(num_particles: int, iterations: int):
  run_particle = RunParticle(args.binary, quiet=True)
  runner_settings = RunnerSettings(dimensions=run_particle.get_num_dimensions(), num_particles=num_particles, iterations=iterations)
  run_particle.print_config()
  performance_logger = PerformanceLogger(run_particle)
  performance_logger.log_settings(runner_settings)
  if args.note is not None:
    performance_logger.note(args.note)
  cost, pos = runner.MyRunner(runner_settings, run_particle, performance_logger=performance_logger).run()
  print(cost)
  return cost, pos

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("--binary", help="binary used for PSO", default=DEFAULT_BINARY)
  parser.add_argument("--note", help="optional note")
  args = parser.parse_args()
  main(args)

