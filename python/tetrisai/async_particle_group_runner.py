import asyncio
import typing

from tetrisai.interfaces.i_run_particle_group import IRunParticleGroup, ParticleGroup
from tetrisai.interfaces.i_run_particle import IRunParticle
from common.common import Particle
import logging

import numpy as np

import multiprocessing
NUM_THREADS = multiprocessing.cpu_count() - 1

class AsyncParticleGroupRunner(IRunParticleGroup):
  def __init__(self, run_evaluator: IRunParticle):
    self._seen = {}
    self._run_evaluator = run_evaluator
    self._logger = logging.getLogger("async_particle_group_runner")

  async def run(self, particle_vs: ParticleGroup, seed:int = None) -> typing.List[float]:
    result = {'result_arr': np.zeros(len(particle_vs))}
    async def particle_worker(i, vs, sem, result):
      await sem.acquire()

      def update_best(particle: Particle, score: float):
        # todo: optimizer tries to minimize score, smh
        result['result_arr'][i] = -score

      async def try_release():
        try:
          await sem.release()
        except Exception:
          pass

      tup = tuple([v for v in vs] + [seed])
      if tup in self._seen:
        val = self._seen[tup]
        self._logger.warning("seen before! result[%d] = %0.8f" % (i, val))
        update_best(vs, val)
        await try_release()
        return
      val = await self._run_evaluator.run(vs, seed=seed)
      self._seen[tup] = val
      update_best(vs, self._seen[tup])
      await try_release()


    global NUM_THREADS
    workers = []
    sem = asyncio.Semaphore(NUM_THREADS)
    for i, vs in enumerate(particle_vs):
      workers.append(particle_worker(i, vs, sem, result))
    await asyncio.wait(workers)
    return result['result_arr']

  def run_sync(self, particle_vs: ParticleGroup, seed:int = None) -> typing.List[float]:
    return asyncio.run(self.run(particle_vs, seed))

