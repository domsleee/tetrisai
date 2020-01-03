from flask import Flask, abort, request
import threading
from interfaces.IGetMoves import IGetMoves
from GetMoves import GetMoves


app = Flask(__name__)
sem = threading.Semaphore(1)
get_moves_service: IGetMoves


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
    piece = payload['piece']
    return get_moves_service.get_moves(board, piece)
  finally:
    sem.release()

  
@app.route('/healthcheck')
def healthcheck():
  return get_moves_service.healthcheck()

if __name__ == '__main__':
  get_moves_service = GetMoves()
  app.run()