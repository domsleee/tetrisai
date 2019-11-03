
from tetrisai.interfaces.i_run_particle import IRunParticle
from tetrisai.runner_settings import RunnerSettings
import datetime
import logging
import os

class PerformanceLogger:
  def __init__(self, run_particle: IRunParticle):
    self._run_particle = run_particle
    self._logger = logging.getLogger('performance_logger')

    now = datetime.datetime.now()
    filename = now.strftime("%Y-%m-%d-%H-%M-%S-performance.log")
    filename = os.path.join("log", filename)
    if not os.path.isdir(os.path.dirname(filename)):
      os.mkdir(os.path.dirname(filename))

    fh = logging.FileHandler(filename)
    formatter = logging.Formatter("%(message)s")
    fh.setFormatter(formatter)
    self._logger.addHandler(fh)
    self._logger.setLevel(logging.DEBUG)
    self._last_time = datetime.datetime.now()

  def log(self, iteration: int, train_score: int, best_particle: any):
    if iteration == 1:
      self._logger.debug("iteration,time_taken,train_score,eval_score,vector")
    now = datetime.datetime.now()
    time_taken = (now - self._last_time).total_seconds()
    eval_score = self._run_particle.run_sync(best_particle, seed=99)
    van_list = [str(x) for x in best_particle]
    self._logger.debug("%d,%0.3f,%0.3f,%0.3f,[%s]" % (iteration, time_taken, train_score, eval_score, ' '.join(van_list)))
    self._last_time = now

  def log_settings(self, settings: RunnerSettings):
    self._logger.debug(settings)

