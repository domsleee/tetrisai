from flask import Blueprint, request, abort
from common import sem, piece_to_int, flaskVars
from GetMovesService import get_moves_service
from flask_cors import CORS, cross_origin
import json

get_moves_given_piece_page = Blueprint('get-moves-given-piece', __name__, template_folder='templates')

d = {}
@get_moves_given_piece_page.route('/get-moves-given-piece', methods=['POST'])
def get_moves_given_piece():
  payload = request.json
  try:
    sem.acquire(timeout=30)
  except Exception:
    abort("failed to get lock")

  try:
    print(payload)
    cachedVal = d.get(str(payload))
    if cachedVal:
      return cachedVal
    board = payload['board']
    piece = piece_to_int(payload['piece'])
    next_piece = piece_to_int(payload['next_piece'])
    line_clears = int(payload['line_clears'])
    get_moves_service.set_num_lines(line_clears)

    result = get_moves_service.get_moves_given_piece(
        board, piece, next_piece)
    if result.result == 'n':
      return {}
    ret = {
      'board': result.nx_board,
      'demo_entries': [str(demo_entry.frame) + " " + demo_entry.action for demo_entry in result.demo_entries],
      'line_clears': result.line_clears
    }
    print(ret)
    r = json.dumps(ret)
    d[str(payload)] = json.dumps(ret)
    return r
  finally:
    sem.release()

