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


@dataclass
class GetMovesResult():
  result: str
  nx_board: str = None
  line_clears: int = None
  demo_entries: typing.List[DemoEntry] = None

class IGetMoves:
  @abc.abstractmethod
  def healthcheck(self) -> bool:
    """check if the service is working
    
    Returns:
      bool -- if the service is running
    """
  
  @abc.abstractmethod
  def get_moves(self, board: str, piece: int, firstMoveDirection: typing.Optional[str]) -> GetMovesResult:
    """[summary]
    
    Arguments:
      board {[bitstring]} -- [description]
      piece {[int]} -- [description]
    
    Returns:
      typing.List[DemoEntry] -- [description]
    """
  
  @abc.abstractmethod
  def get_moves_given_piece(self, board: str, piece: int, nextPiece: int, firstMoveDirection: str) -> GetMovesResult:
    """[summary]
    
    Arguments:
        board {str} -- [description]
        piece {int} -- [description]
        nextPiece {int} -- [description]
        firstMoveDirection {str} -- [description]
    
    Returns:
        GetMovesResult -- [description]
    """
  
  
  @abc.abstractmethod
  def set_num_lines(self, num_lines:int) -> None:
    """[summary]
    
    Arguments:
        num_lines {int} -- [description]
    """
  
  @abc.abstractmethod
  def get_num_lines(self) -> int:
    """[summary]
    
    Returns:
        int -- [description]
    """
