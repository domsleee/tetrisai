import abc
import typing
import numpy
from common.common import Particle

ParticleGroup = numpy.ndarray # Particle

class IRunParticleGroup:
  @abc.abstractmethod
  def run_sync(self, particle_group: ParticleGroup, seed: int = None) -> typing.List[float]:
    """
    vs: particle to take in
    """

  @abc.abstractmethod
  async def run(self, vs: ParticleGroup, seed: int = None) -> typing.List[float]:
    """
    vs: particle to take in
    """
