import asyncio
import os
import typing

from tetrisai.interfaces.i_run_particle import IRunParticle
from common.common import Particle


class RunParticle(IRunParticle):
  def __init__(self, binary: str):
    self._binary = binary
    if not os.access(binary, os.X_OK):
      raise ValueError("Binary '%s' must be executable" % binary)
  
  async def run(self, vs: Particle, seed: int = None):
    args = [self._binary] + [str(v) for v in vs]
    if seed:
      args += [str(seed)]
    exec_str = ' '.join(args)
    return await self._run_cmd(exec_str)

  async def _run_cmd(self, cmd: str):
    stdout, stderr = await self._run_cmd_base(cmd)
    try:
      return float(stdout.decode("utf-8"))
    except Exception as ex:
      print("EXCEPTION!!!!")
      print(cmd)
      print(stdout)
      print(stdout.decode("utf-8"))
      print(stderr)
      raise ex
  
  async def _run_cmd_base(self, cmd: str):
    proc = await asyncio.create_subprocess_shell(
      cmd,
      stdout=asyncio.subprocess.PIPE,
      stderr=asyncio.subprocess.PIPE)
    stdout, stderr = await proc.communicate()
    return (stdout, stderr)

  def run_sync(self, vs: Particle, seed: int = None):
    return asyncio.run(self.run(vs, seed))
  
  def get_num_dimensions(self) -> int:
    async def fn():
      stdout, _ = await self._run_cmd_base(f'{self._binary} -d')
      return int(stdout.decode('utf-8').strip())
    return asyncio.run(fn())

  def get_config(self) -> str:
    async def fn():
      stdout, _ = await self._run_cmd_base(f'{self._binary} -c')
      return stdout.decode('utf-8')
    return asyncio.run(fn())

  def print_config(self):
    print(self.get_config())
