import typing

NUM_ROWS = 20
NUM_COLUMNS = 10

class BitBoard:
  @staticmethod
  def fromBoard(arr: typing.List[str]) -> str:
    if len(arr) != NUM_ROWS:
      raise ValueError("incorrect row size")
    ret = ""
    for row in arr:
      if len(row) != NUM_COLUMNS:
        raise ValueError("incorrect column size")
      ret += row
    return ret