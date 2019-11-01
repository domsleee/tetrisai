import numpy as np
from shell import shell
import os
import argparse
import pyswarms as ps
from random import randint
import asyncio
import logging
import multiprocessing

import matplotlib.pyplot as plt
from pyswarms.utils.plotters import (plot_cost_history, plot_contour, plot_surface)


glob_best = 0.0
NUM_THREADS = multiprocessing.cpu_count() - 1

print("NUM_THREADS:", NUM_THREADS)

LOCAL = False
NUM_PARTICLES = 100
NUM_NEIGHBOURS = 100
NUM_ITERATIONS = 6900

logging.basicConfig()

class MyRunner:
  def __init__(self, binary: str):
    self._binary = binary
    if not os.access(binary, os.X_OK):
      raise ValueError("Binary must be executable")
    self._logger = logging.getLogger(__package__)
    self._seen = {}

  def run(self):
    global LOCAL, NUM_NEIGHBOURS, NUM_PARTICLES, NUM_ITERATIONS
    options = {'c1': 2.5, 'c2': 2.5, 'w': 0.92}
    if LOCAL:
      options = {'c1': 2, 'c2': 2, 'w': 0.728}
      options['k'] = NUM_NEIGHBOURS
      options['p'] = 2
      optimizer = ps.single.LocalBestPSO(n_particles=NUM_PARTICLES, dimensions=17, options=options, velocity_clamp=(0,5))
    else:
      optimizer = ps.single.GlobalBestPSO(n_particles=NUM_PARTICLES, dimensions=17, options=options)
    
    
    cost, pos = optimizer.optimize(self.run_particle, iters=NUM_ITERATIONS)
    self._show_plot(optimizer)
    print(cost, pos)


  def _show_plot(self, optimizer):
    plot_cost_history(cost_history=optimizer.cost_history)
    plt.show()


  def run_particle(self, particle_vs):
    result = np.zeros(len(particle_vs))
    async def my_worker(i, vs, sem):
      global glob_best
      await sem.acquire()

      async def try_release():
        try:
          await sem.release()
        except Exception:
          pass

      tup = tuple(vs)
      if tup in self._seen:
        val = self._seen[tup]
        self._logger.warning("seen before! result[%d] = %0.8f" % (i, val))
        result[i] = val
        await try_release()
        return
      val = await self.run_one(vs)
      if val > glob_best:
        glob_best = val
        self._logger.info("new glob best! %0.8f" % glob_best)
        self._logger.info(vs)
      result[i] = -val
      self._seen[tup] = -val
      self._logger.debug(vs)
      self._logger.debug("result[%d] = %0.8f\n" % (i, result[i]))
      await try_release()

    async def wait_all():
      global NUM_THREADS
      workers = []
      sem = asyncio.Semaphore(NUM_THREADS)
      for i, vs in enumerate(particle_vs):
        workers.append(my_worker(i, vs, sem))
      await asyncio.wait(workers)
    
    #print(particle_vs.shape)
    asyncio.run(wait_all())
    return result
  
  async def run_one(self, vs):
    args = [self._binary] + [str(v) for v in vs]
    exec_str = ' '.join(args)
    return await async_run(exec_str)

async def async_run(cmd):
  proc = await asyncio.create_subprocess_shell(
    cmd,
    stdout=asyncio.subprocess.PIPE,
    stderr=asyncio.subprocess.PIPE)

  stdout, stderr = await proc.communicate()
  return float(stdout.decode("utf-8"))


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("binary", help="binary used for PSO")
  args = parser.parse_args()
  MyRunner(args.binary).run()

