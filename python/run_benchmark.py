#!/usr/bin/env python3
from tetrisai.runner_settings import RunnerSettings
from tetrisai.run_particle import RunParticle
import datetime
import typing
from common.common import EW_BENCH_BINARY


BEST_V = "[19.64783140930567 9.628218376087286 13.454904236734368 -2.02793077057305 6.670428230209634 10.85252370833494 15.132272899283567 -1.6308387741656925 -1.7662737515285953 21.238094077140975 14.433879473215011 -3.8388314800699845 -3.7678183941852734 -8.94126576437808 -15.097047127080824 1.471153977982912 0.7640825103878978]"

def main():
  r = RunParticle(binary=EW_BENCH_BINARY)
  vs = read_v(BEST_V)
  print("Running one particle...")
  old = datetime.datetime.now()
  result = r.run_sync(vs)
  new = datetime.datetime.now()
  delta = (new-old).total_seconds()
  print("Result: %0.3f" % result)
  print("Time taken: %0.3f" % delta)

def read_v(s: str) -> typing.List[float]:
  s = s.replace('[', '')
  s = s.replace(']', '')
  return [float(x) for x in s.split(' ')]

if __name__ == '__main__':
  main()
