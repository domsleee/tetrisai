from interfaces.IGetMoves import IGetMoves, DemoEntry, BoardT
import subprocess
import typing
import os

DIR = "/Users/dom/Documents/git/tetrisAI/.docker/get-move-service/src"
DEFAULT_BINARY = os.path.join(DIR, "external_bin", "ew_get_moves")

class GetMoves(IGetMoves):
  def __init__(self, binary: str = DEFAULT_BINARY):
    self._binary = binary

  # todo: use context manager
  def close(self):
    self._process = None

  def healthcheck(self) -> bool:
    return True

  def get_moves(self, board: str, piece: int) -> typing.Tuple[BoardT, typing.List[DemoEntry]]:    
    return board, []
