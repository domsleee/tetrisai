import asyncio
import os
import typing

from tetrisai.interfaces.i_run_evaluator import IRunEvaluator

class RunEvaluator(IRunEvaluator):
  def __init__(self, binary: str):
    self._binary = binary
    if not os.access(binary, os.X_OK):
      raise ValueError("Binary must be executable")
  
  async def run(self, vs: typing.List[float], seed: int = None):
    args = [self._binary] + [str(v) for v in vs]
    exec_str = ' '.join(args)
    return await self._run_cmd(exec_str)

  async def _run_cmd(self, cmd: str):
    proc = await asyncio.create_subprocess_shell(
      cmd,
      stdout=asyncio.subprocess.PIPE,
      stderr=asyncio.subprocess.PIPE)

    stdout, stderr = await proc.communicate()
    return float(stdout.decode("utf-8"))
  
  def run_sync(self, vs: typing.List[float], seed: int = None):
    return asyncio.run(self.run(vs, seed))
