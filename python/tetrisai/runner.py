import numpy as np
import os
import pyswarms as ps
from random import randint
import asyncio
import logging
import multiprocessing
from tetrisai.runner_settings import RunnerSettings
from tetrisai.performance_logger import PerformanceLogger
from tetrisai.run_evaluator import RunEvaluator
NUM_THREADS = multiprocessing.cpu_count() - 1


class MyRunner:
  def __init__(self, binary: str, runner_settings: RunnerSettings):
    self._binary = binary
    if not os.access(binary, os.X_OK):
      raise ValueError("Binary must be executable")
    self._logger = logging.getLogger(__package__)
    self._seen = {}
    self._glob_best = 0.0
    self._best_particle = []
    self._runner_settings = runner_settings
    self._run_evaluator = RunEvaluator(binary)
    self._performance_logger = PerformanceLogger(self._run_evaluator)
    self._iteration = 0

    if self._runner_settings.local:
      self._optimizer = ps.single.LocalBestPSO(**self._runner_settings.get_optimizer_options())
    else:
      self._optimizer = ps.single.GlobalBestPSO(**self._runner_settings.get_optimizer_options())
    self._performance_logger.log_settings(self._runner_settings)

  def run(self):
    self._iteration = 0
    cost, pos = self._optimizer.optimize(self.run_particle_group, iters=self._runner_settings.iterations)
    #self._show_plot(optimizer)
    return self._optimizer

  def run_particle_group(self, particle_vs):
    self._iteration += 1
    result = np.zeros(len(particle_vs))
    async def particle_worker(i, vs, sem):
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
      val = await self._run_evaluator.run(vs)
      if val > self._glob_best:
        self._glob_best = val
        self._best_particle = vs
        self._logger.info("new glob best! %0.8f" % self._glob_best)
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
        workers.append(particle_worker(i, vs, sem))
      await asyncio.wait(workers)
    
    #print(particle_vs.shape)
    asyncio.run(wait_all())
    self._performance_logger.log(iteration=self._iteration, train_score=self._glob_best, best_particle=self._best_particle)
    return result
