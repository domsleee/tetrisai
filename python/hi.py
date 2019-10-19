import numpy as np
from shell import shell
import os
import argparse
import pyswarms as ps
from random import randint

class MyRunner:
  def __init__(self, binary: str):
    self._binary = binary
    if not os.access(binary, os.X_OK):
      raise ValueError("Binary must be executable")

  def run(self):
    options = {'c1': 0.5, 'c2': 0.3, 'w': 0.9}
    optimizer = ps.single.GlobalBestPSO(n_particles=200, dimensions=17, options=options, velocity_clamp=(0,5))
    cost, pos = optimizer.optimize(self.run_particle, iters=1000)
    print(cost, pos)

  def run_particle(self, particle_vs):
    result = np.zeros(len(particle_vs))
    for i, vs in enumerate(particle_vs):
      result[i] = self.run_one(vs)
      print("result[%d] = %0.8f\n" % (i, result[i]))
    return result
  
  def run_one(self, vs):
    args = [self._binary] + [str(v) for v in vs]
    exec_str = ' '.join(args)
    ex = shell(exec_str)
    return float(ex.output()[0])



if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("binary", help="binary used for PSO")
  args = parser.parse_args()
  MyRunner(args.binary).run()

