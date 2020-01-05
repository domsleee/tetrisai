from interfaces.IGetMoves import IGetMoves, DemoEntry, BoardT
import subprocess
import typing
import os
import select
import time
from IOReader import IOReader
from SimpleProcess import SimpleProcess
import asyncio
import logging

DIR = "/Users/dom/Documents/git/tetrisAI/.docker/get-move-service/src"
DEFAULT_BINARY = os.path.join(DIR, "external_bin", "ew_get_moves")

class GetMoves(IGetMoves):
  DEFAULT_TIMEOUT = 1.0

  def __init__(self, binary: str = DEFAULT_BINARY):
    self._binary = binary
    self._process = None
    self._logger = logging.Logger("GetMoves")
    
  async def init(self):
    self._process = SimpleProcess([self._binary])
    await self._process.init()

  def close(self):
    self._process.send_line(b'q\n')
    self._process.close()
  
  def healthcheck(self) -> bool:
    self._send_str(b'h\n')
    nx_line = self._process.read_line()
    return nx_line == "OK"

  def get_moves(self, board: str, piece: int) -> typing.Tuple[str, BoardT, typing.List[DemoEntry]]:    
    self._send_str(b'g\n')
    self._send_str((str(piece) + '\n').encode('utf-8'))
    self._send_str((board + '\n').encode('utf-8'))

    result = self._process_line(self._process.read_line(), "result")
    if result == "no moves":
      return "n", None, None

    num_moves = int(self._process_line(self._process.read_line(), "num moves"))
    # todo: process mvoes
    self._logger.debug("num_moves: %d" % num_moves)
    nx_board = self._process_line(self._process.read_line(), "board")
    self._process_line(self._process.read_line(), "line clears")
    assert(self._process.is_empty())
    return "r", nx_board, []
  
  def _process_line(self, s: str, lhs: str) -> str:
    col_split = s.split(':')
    if len(col_split) != 2:
      raise ValueError("Invalid line '%s'" % s)
    my_lhs = col_split[0].strip()
    if my_lhs != lhs:
      raise ValueError("Invalid line. Expected lhs to be '%s' in '%s'" % (lhs, s))
    return col_split[1].strip()
    
  def _send_str(self, b: bytes):
    self._process.send_line(b)

