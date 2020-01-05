import abc
import typing

# frame, enum, isMove
DemoEntry = typing.Tuple[int, int, bool]
BoardT = str

class IGetMoves:
  @abc.abstractmethod
  def healthcheck(self) -> bool:
    """check if the service is working
    
    Returns:
      bool -- if the service is running
    """
  
  @abc.abstractmethod
  def get_moves(self, board: str, piece: int) -> typing.Tuple[BoardT, typing.List[DemoEntry]]:
    """[summary]
    
    Arguments:
      board {[bitstring]} -- [description]
      piece {[int]} -- [description]
    
    Returns:
      typing.List[DemoEntry] -- [description]
    """
