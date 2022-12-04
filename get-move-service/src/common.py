import threading
from flask_caching import Cache
from dataclasses import dataclass

@dataclass
class FlaskVars:
  cache: Cache = None

flaskVars = FlaskVars()

def piece_to_int(piece: str) -> int:
  ind = [
    "I_PIECE",
    "L_PIECE",
    "J_PIECE",
    "T_PIECE",
    "Z_PIECE",
    "S_PIECE",
    "O_PIECE"
  ].index(piece)
  if ind == -1:
    raise ValueError("unknown piece")
  return ind

sem = threading.Semaphore(1)
