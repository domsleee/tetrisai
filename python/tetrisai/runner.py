import numpy as np
import os
import pyswarms as ps
from random import randint
import asyncio
import logging
import multiprocessing
from tetrisai.runner_settings import RunnerSettings
from tetrisai.performance_logger import PerformanceLogger
from tetrisai.interfaces.i_run_particle import IRunParticle

from tetrisai.async_particle_group_runner import ParticleGroup, AsyncParticleGroupRunner

class MyRunner:
  def __init__(self, runner_settings: RunnerSettings, run_particle: IRunParticle, performance_logger: PerformanceLogger=None):
    self._logger = logging.getLogger(__package__)
    self._seen = {}
    self._glob_best = 0.0
    self._best_particle = []
    self._runner_settings = runner_settings
    self._performance_logger = performance_logger
    if performance_logger is None:
      self._performance_logger = PerformanceLogger(run_particle)
    self._particle_group_runner = AsyncParticleGroupRunner(run_particle)
    self._iteration = 0

  def run(self):
    self._iteration = 0
    if self._runner_settings.local:
      optimizer = ps.single.LocalBestPSO(**self._runner_settings.get_optimizer_options())
    else:
      optimizer = ps.single.GlobalBestPSO(**self._runner_settings.get_optimizer_options())
    cost, pos = optimizer.optimize(self._run_particle_group, iters=self._runner_settings.iterations)
    #self._show_plot(optimizer)
    return optimizer
  
  def _run_particle_group(self, particle_vs: ParticleGroup):
    self._iteration += 1
    result_arr = self._particle_group_runner.run_sync(particle_vs)
    for i, score in enumerate(result_arr):
      my_score = -score
      if my_score > self._glob_best:
        self._glob_best = my_score
        self._best_particle = particle_vs[i]
        self._logger.info("new glob best! %0.8f" % self._glob_best)
        self._logger.info(self._best_particle)
    self._performance_logger.log(iteration=self._iteration, train_score=self._glob_best, best_particle=self._best_particle)
    return result_arr
