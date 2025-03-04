#!/usr/bin/env python

from flask import Flask, abort, request, jsonify
import threading
from GetMoves import GetMoves
from common import piece_to_int
import asyncio
import logging
import dataclasses
import json
import os
from common import sem
from GetMovesService import get_moves_service

from flask_cors import CORS, cross_origin
from flask_caching import Cache
from common import flaskVars
app = Flask(__name__)
app.config['CORS_HEADERS'] = 'Content-Type'
app.config['CACHE_TYPE'] = 'SimpleCache'
#app.config['DEBUG'] = True
flaskVars.cache = Cache(app)
cors = CORS(app)
loop = asyncio.get_event_loop()
DIR = os.path.dirname(os.path.realpath(__file__))

logging.basicConfig(level=logging.DEBUG)

from routes.get_moves_given_piece import get_moves_given_piece_page
app.register_blueprint(get_moves_given_piece_page)


@app.route('/')
def hello():
    return "Hello World!"


@app.route('/get-moves', methods=['POST'])
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
      raise ValueError(
        f"Unexpected first_move_direction: {first_move_direction}")

    line_clears = 0
    if 'line_clears' in payload:
      line_clears = int(payload['line_clears'])
      print("set line clears to ", line_clears)
    get_moves_service.set_num_lines(line_clears)
    print("REQUEST")
    print(payload)
    result = get_moves_service.get_moves(
        board, piece, first_move_direction)
    if result.result == "n":
      return {}
    print(result.result)

    ret = {
      'board': result.nx_board,
      'demo_entries': [str(demo_entry.frame) + " " + demo_entry.action for demo_entry in result.demo_entries],
      'line_clears': result.line_clears
    }
    print(ret)
    cache[payload] = json.dumps(ret)
    return cache[payload]
  finally:
    sem.release()

@app.route('/get-file', methods=['POST'])
def get_file_handler():
  payload = request.json
  file = payload['file']
  my_file = os.path.join(DIR, '..', '..', '..', 'save_states', file)
  with open(my_file, 'r') as fp:
    s = fp.read()
  return json.dumps({
    'result': s
  })


@app.route('/healthcheck')
def healthcheck():
  return get_moves_service.healthcheck()


if __name__ == '__main__':
  #init_get_moves_service(loop)
  app.run(host='0.0.0.0')
