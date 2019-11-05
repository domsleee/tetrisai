import asyncio
import os
import typing

from tetrisai.interfaces.i_run_particle import IRunParticle
from common.common import Particle
import logging


class RunParticleWithData(IRunParticle):


  def __init__(self, binary: str):
    self._glob = {}
    self._ct = {}
    self._stdouts = []
    self._binary = binary
    if not os.access(binary, os.X_OK):
      raise ValueError("Binary '%s' must be executable" % binary)
    self._logger = logging.getLogger("run_particle_with_data")
  
  async def run(self, vs: Particle, seed: int = None):
    args = [self._binary] + [str(v) for v in vs]
    if seed:
      args += [str(seed)]
    exec_str = ' '.join(args)
    self._logger.info(exec_str)
    return await self._run_cmd(exec_str)

  async def _run_cmd(self, cmd: str):
    proc = await asyncio.create_subprocess_shell(
      cmd,
      stdout=asyncio.subprocess.PIPE,
      stderr=asyncio.subprocess.PIPE)

    stdout, stderr = await proc.communicate()
    my_s = stdout.decode("utf-8")
    spl = my_s.split("\n")

    i = 0
    while i < (len(spl)-2) - 2:
      board = spl[i]
      blockType = spl[i+1]
      moves = spl[i+2]
      if (board, blockType) not in self._glob:
        self._glob[(board, blockType)] = moves
        self._ct[(board, blockType)] = 1
      else:
        # verify that the moves match
        if self._glob[(board, blockType)] != moves:
          raise ValueError("fatal. Error in your C program")
        self._ct[(board, blockType)] += 1
      i += 3
    self._stdouts.append(my_s)
    return float(spl[-2])
    
  def run_sync(self, vs: Particle, seed: int = None):
    return asyncio.run(self.run(vs, seed))

  def get_moves_and_count(self):
    return self._glob, self._ct

  def get_stdouts(self):
    return self._stdouts
