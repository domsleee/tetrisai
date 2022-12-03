from interfaces.IGetMoves import IGetMoves, DemoEntry, BoardT, GetMovesResult
import typing
import os
from SimpleProcess import SimpleProcess
import logging
import asyncio
import dataclasses

DIR = os.path.dirname(os.path.realpath(__file__))
BAZEL_BIN_DIR = os.path.join(DIR, '..', '..', 'cpp', 'bazel-bin')
BAZEL_BINARY = os.path.join(BAZEL_BIN_DIR, "src", "pso", "main", "get_moves")
DEFAULT_BINARY = os.environ.get("DEFAULT_BINARY", BAZEL_BINARY)# os.path.join(DIR, "external_bin", "get_moves")

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

  def get_moves(self, board: str, piece: int, firstMoveDirection: typing.Optional[str] = None) -> GetMovesResult:    
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

    demo_entries = self._read_num_moves()
    nx_board = self._process_line(self._process.read_line(), "board")
    line_clears = self._process_line(self._process.read_line(), "line clears")
    assert(self._process.is_empty())
    ok = self._process.read_line()
    if ok != "OK":
      raise ValueError("Expected OK, got '%s'" % ok)
    return GetMovesResult(result="r", nx_board=nx_board, line_clears=line_clears, demo_entries=demo_entries)
  
  def _read_num_moves(self):
    demo_entries = []
    num_moves = int(self._process_line(self._process.read_line(), "num moves"))
    for i in range(num_moves):
      move = self._process.read_line()
      [frame, action] = move.split(' ')
      frame = int(frame)
      demo_entries.append(DemoEntry(frame=frame, action=action))
    self._logger.debug("num_moves: %d" % num_moves)
    return demo_entries

  def get_moves_given_piece(self, board: str, piece: int, nextPiece: int) -> GetMovesResult:
    self._send_str(b'p\n')
    self._send_str((str(piece) + '\n').encode('utf-8'))
    self._send_str((str(nextPiece) + '\n').encode('utf-8'))
    self._send_str(('.\n').encode('utf-8'))
    self._send_str((board + '\n').encode('utf-8'))

    result = self._process_line(self._process.read_line(), "result")
    if result == "no moves":
      return GetMovesResult(result="n")
    elif result != "moves":
      raise ValueError(f"Unknown result '{result}'")

    demo_entries = self._read_num_moves()
    nx_board = self._process_line(self._process.read_line(), "board")
    line_clears = self._process_line(self._process.read_line(), "line clears")
    ok = self._process.read_line()
    if ok != "OK":
      raise ValueError("Expected OK, got '%s'" % ok)
    return GetMovesResult(result="r", nx_board=nx_board, line_clears=line_clears, demo_entries=demo_entries)

  def set_num_lines(self, num_lines:int):
    self._send_str(b'l\n')
    self._send_str((str(num_lines) + '\n').encode('utf-8'))
  
  def get_num_lines(self) -> int:
    self._send_str(b'L\n')
    num_lines = int(self._process_line(self._process.read_line(), "num lines"))
    return num_lines

  def _process_line(self, s: str, lhs: str) -> str:
    col_split = s.split(':')
    if len(col_split) != 2:
      while 1:
        line = self._process.read_line()
        if line != "":
          print(line)
      raise ValueError("Invalid line (expected a colon) '%s'" % s)
    my_lhs = col_split[0].strip()
    if my_lhs != lhs:
      raise ValueError("Invalid line. Expected lhs to be '%s' in '%s'" % (lhs, s))
    return col_split[1].strip()
    
  def _send_str(self, b: bytes):
    self._logger.info("send_line %s" % b)
    self._process.send_line(b)
