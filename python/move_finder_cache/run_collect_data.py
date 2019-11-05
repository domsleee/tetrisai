#!/usr/bin/env python3
from tetrisai.runner_settings import RunnerSettings
from tetrisai.run_particle import RunParticle
import datetime
import typing
from common.common import MOVE_FINDER_CACHE_BINARY
from tetrisai import runner
from tetrisai.async_particle_group_runner import ParticleGroup, AsyncParticleGroupRunner
from tetrisai.run_particle_with_data import RunParticleWithData


"""
NUM_RUNS = 2 # 5
NUM_PARTICLES = 2 # 100
NUM_ITERATIONS = 1 # 3
"""

NUM_RUNS = 5
NUM_PARTICLES = 100
NUM_ITERATIONS = 3

def main():
  ans = {}
  stdouts = []
  for _ in range(NUM_RUNS):
    runner_settings = RunnerSettings(num_particles=NUM_PARTICLES, iterations=NUM_ITERATIONS)
    run_particle_with_data = RunParticleWithData(MOVE_FINDER_CACHE_BINARY)
    my_runner = runner.MyRunner(runner_settings, run_particle_with_data)
    my_runner.run()
    _, ct = run_particle_with_data.get_moves_and_count()
    stdouts += run_particle_with_data.get_stdouts()
    for key, val in ct.items():
      if key not in ans:
        ans[key] = val
      else:
        ans[key] += val

  with open("mystdouts.out", "w") as fp:
    print(stdouts, file=fp)

  sorted_ans = sorted(ans.items(), key=lambda kv: kv[1], reverse=True)
  with open("myout.out", "w") as fp:
    for (board, blockType), ct in sorted_ans:
      print(board, file=fp)
      print(blockType, file=fp)
      print(ct, file=fp)


if __name__ == '__main__':
  main()
