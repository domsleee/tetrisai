import abc
import typing

class IRunEvaluator:
  @abc.abstractmethod
  def run_sync(self, vs: typing.List[float], seed: int = None) -> float:
    """
    vs: particle to take in
    """

  @abc.abstractmethod
  async def run(self, vs: typing.List[float], seed: int = None) -> float:
    """
    vs: particle to take in
    """
