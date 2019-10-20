import numpy as np
from shell import shell
import os
import argparse
import pyswarms as ps
from random import randint
import asyncio
import logging
import multiprocessing
NUM_THREADS = multiprocessing.cpu_count()

print("NUM_THREADS:", NUM_THREADS)

LOCAL = False
NUM_PARTICLES = 1000
NUM_NEIGHBOURS = 2
NUM_ITERATIONS = 1000

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
    options = {'c1': 0.5, 'c2': 0.3, 'w': 0.9}
    if LOCAL:
      options['k'] = NUM_NEIGHBOURS
      options['p'] = 2
      optimizer = ps.single.LocalBestPSO(n_particles=NUM_PARTICLES, dimensions=17, options=options, velocity_clamp=(-5,5))
    else:
      optimizer = ps.single.GlobalBestPSO(n_particles=NUM_PARTICLES, dimensions=17, options=options)
    cost, pos = optimizer.optimize(self.run_particle, iters=NUM_ITERATIONS)
    print(cost, pos)

  def run_particle(self, particle_vs):
    result = np.zeros(len(particle_vs))
    async def my_worker(i, vs, sem):
      await sem.acquire()
      tup = tuple(vs)
      if tup in self._seen:
        self._logger.warning("seen before!")
        return self._seen[tup]
      val = await self.run_one(vs)
      result[i] = -val
      self._seen[tup] = -val
      self._logger.debug(vs)
      self._logger.info("result[%d] = %0.8f\n" % (i, result[i]))
      try:
        await sem.release()
      except Exception:
        pass

    async def wait_all():
      global NUM_THREADS
      workers = []
      sem = asyncio.Semaphore(NUM_THREADS)
      for i, vs in enumerate(particle_vs):
        workers.append(my_worker(i, vs, sem))
      await asyncio.wait(workers)
    
    print(particle_vs.shape)
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

