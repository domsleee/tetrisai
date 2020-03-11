from flask import Blueprint, request, abort
from common import sem, piece_to_int
from GetMovesService import get_moves_service
from flask_cors import CORS, cross_origin
import json

get_moves_given_piece_page = Blueprint('get-moves-given-piece', __name__, template_folder='templates')

@get_moves_given_piece_page.route('/get-moves-given-piece', methods=['POST'])
@cross_origin()
def get_moves_given_piece():
  print(request)
  payload = request.json
  try:
    sem.acquire(timeout=30)
  except Exception:
    abort("failed to get lock")

  try:
    print("get-moves-given-piece REQUEST")
    print(payload)
    board = payload['board']
    piece = piece_to_int(payload['piece'])
    next_piece = piece_to_int(payload['next_piece'])
    first_move_direction = payload.get('first_move_direction')
    if first_move_direction not in [None, 'NONE', 'LEFT', 'RIGHT']:
      raise ValueError(
        f"Unexpected first_move_direction: {first_move_direction}")
    line_clears = int(payload['line_clears'])
    get_moves_service.set_num_lines(line_clears)

    result = get_moves_service.get_moves_given_piece(
        board, piece, next_piece, first_move_direction)
    ret = {
      'board': result.nx_board,
      'demo_entries': [str(demo_entry.frame) + " " + demo_entry.action for demo_entry in result.demo_entries],
      'line_clears': result.line_clears
    }
    print(ret)
    return json.dumps(ret)
  finally:
    sem.release()

