import abc
import typing
import enum

from dataclasses import dataclass

ACTIONS = {
  "LEFT",
  "RIGHT",
  "UP",
  "DOWN",
  "ROTATE_AC",
  "ROTATE_C"
}

@dataclass
class DemoEntry:
  action: str
  frame: int
  def __init_(self, action:str, frame:int):
    assert(action in ACTIONS)
    self.action = action
    self.frame = frame

BoardT = str

class IGetMoves:
  @abc.abstractmethod
  def healthcheck(self) -> bool:
    """check if the service is working
    
    Returns:
      bool -- if the service is running
    """
  
  @abc.abstractmethod
  def get_moves(self, board: str, piece: int, firstMoveDirection: typing.Optional[str]) -> typing.Tuple[BoardT, typing.List[DemoEntry]]:
    """[summary]
    
    Arguments:
      board {[bitstring]} -- [description]
      piece {[int]} -- [description]
    
    Returns:
      typing.List[DemoEntry] -- [description]
    """
