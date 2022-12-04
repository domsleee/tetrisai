#!/usr/bin/env python
from common import sem, piece_to_int


def toInput(payload):
  print('l')
  print(payload['line_clears'])
  print('p')
  print(piece_to_int(payload['piece']))
  print(piece_to_int(payload['next_piece']))
  print('.')
  print(payload['board'])
  print('q')


payload = {'board': '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000110000000011000000001100000000111110000011111100111111110011111111101111111110111111111011111111101111', 'piece': 'S_PIECE', 'next_piece': 'L_PIECE', 'line_clears': 131}
toInput(payload)