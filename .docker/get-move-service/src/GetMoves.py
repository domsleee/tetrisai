from interfaces.IGetMoves import IGetMoves, DemoEntry, BoardT
import typing
import os
from SimpleProcess import SimpleProcess
import logging
import asyncio
import dataclasses

DIR = "/Users/dom/Documents/git/tetrisAI/.docker/get-move-service/src"
DEFAULT_BINARY = os.path.join(DIR, "external_bin", "ew_get_moves")


@dataclasses.dataclass
class GetMovesResult():
  result: str
  nx_board: str = None
  lines_cleared: int = None
  demo_entries: typing.List[DemoEntry] = None

class GetMoves(IGetMoves):
  DEFAULT_TIMEOUT = 1.0

  def __init__(self, binary: str = DEFAULT_BINARY, loop=asyncio.get_event_loop()):
    self._binary = binary
    self._process = None
    self._logger = logging.Logger("GetMoves")
    self._logger.setLevel(logging.DEBUG)
    self._loop = loop
    self._loop.run_until_complete(self.secondInit())
  
  async def secondInit(self):
    self._process = SimpleProcess([self._binary], loop=self._loop)
    await self._process.init()

  def close(self):
    self._process.send_line(b'q\n')
    self._process.close()
  
  def healthcheck(self) -> bool:
    self._send_str(b'h\n')
    nx_line = self._process.read_line()
    return nx_line == "OK"

  def get_moves(self, board: str, piece: int, firstMoveDirection: typing.Optional[str] = None) -> typing.Tuple[str, BoardT, typing.List[DemoEntry]]:    
    if firstMoveDirection:
      self._send_str(b'k\n')
    else:
      self._send_str(b'g\n')
    self._send_str((str(piece) + '\n').encode('utf-8'))
    if firstMoveDirection:
      self._send_str((firstMoveDirection[0] + '\n').encode('utf-8'))
    if len(board) != 200:
      raise ValueError("Board string '%s' incorrect length (expect 200)" % board)
    self._send_str((board + '\n').encode('utf-8'))

    result = self._process_line(self._process.read_line(), "result")
    if result == "no moves":
      return GetMovesResult(result="n")
    elif result != "moves":
      raise ValueError(f"wtf is {result}")

    demo_entries = []
    num_moves = int(self._process_line(self._process.read_line(), "num moves"))
    for i in range(num_moves):
      move = self._process.read_line()
      [frame, action] = move.split(' ')
      frame = int(frame)
      demo_entries.append(DemoEntry(frame=frame, action=action))
    self._logger.debug("num_moves: %d" % num_moves)
    nx_board = self._process_line(self._process.read_line(), "board")
    lines_cleared = self._process_line(self._process.read_line(), "line clears")
    assert(self._process.is_empty())
    ok = self._process.read_line()
    if ok != "OK":
      raise ValueError("Expected OK, got '%s'" % ok)
    return GetMovesResult(result="r", nx_board=nx_board, lines_cleared=lines_cleared, demo_entries=demo_entries)
  
  def _process_line(self, s: str, lhs: str) -> str:
    col_split = s.split(':')
    if len(col_split) != 2:
      raise ValueError("Invalid line (expected a colon) '%s'" % s)
    my_lhs = col_split[0].strip()
    if my_lhs != lhs:
      raise ValueError("Invalid line. Expected lhs to be '%s' in '%s'" % (lhs, s))
    return col_split[1].strip()
    
  def _send_str(self, b: bytes):
    self._logger.info("send_line %s" % b)
    self._process.send_line(b)

