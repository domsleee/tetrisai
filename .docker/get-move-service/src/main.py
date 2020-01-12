from flask import Flask, abort, request, jsonify
import threading
from interfaces.IGetMoves import IGetMoves
from GetMoves import GetMoves
from common import piece_to_int
import asyncio
import logging
import dataclasses
import json

from flask_cors import CORS, cross_origin

app = Flask(__name__)
sem = threading.Semaphore(1)
get_moves_service: IGetMoves
cors = CORS(app)
app.config['CORS_HEADERS'] = 'Content-Type'
loop = asyncio.get_event_loop()

logging.basicConfig(level=logging.DEBUG)

@app.route('/')
@cross_origin()
def hello():
  return "Hello World!"


@app.route('/get-moves', methods=['POST'])
@cross_origin()
def get_move_handler():
  payload = request.json
  try:
    sem.acquire(timeout=30)
  except Exception:
    abort("failed to get lock")
  
  try:
    board = payload['board']
    piece = piece_to_int(payload['piece'])
    first_move_direction = payload.get('first_move_direction')
    if first_move_direction not in [None, 'NONE', 'LEFT', 'RIGHT']:
      raise ValueError(f"Unexpected first_move_direction: {first_move_direction}")
    print("REQUEST")
    print(payload)
    r, board, demo_entries = get_moves_service.get_moves(board, piece, first_move_direction)
    ret = {
      'board': board,
      'demo_entries': [str(demo_entry.frame) + " " + demo_entry.action for demo_entry in demo_entries]
    }
    print(ret)
    return json.dumps(ret)
  finally:
    sem.release()


  
@app.route('/healthcheck')
def healthcheck():
  return get_moves_service.healthcheck()

if __name__ == '__main__':
  get_moves_service = GetMoves(loop=loop)
  app.run()