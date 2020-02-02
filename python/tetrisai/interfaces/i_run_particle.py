import abc
import typing

from common.common import Particle

class IRunParticle:
  @abc.abstractmethod
  def run_sync(self, vs: Particle, seed: int = None) -> float:
    """
    vs: particle to take in
    """

  @abc.abstractmethod
  async def run(self, vs: Particle, seed: int = None) -> float:
    """
    vs: particle to take in
    """
    
  @abc.abstractmethod
  def get_config(self) -> str:
    """
    
    """
