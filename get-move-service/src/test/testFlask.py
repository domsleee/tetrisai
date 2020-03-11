#!/usr/bin/env python3

import unittest
from main import app
import logging
import json

from test.defs import FIRST_MOVE_FRAME


class TestFlask(unittest.TestCase):
  def setUp(self):
    app.config['TESTING'] = True
    app.config['WTF_CSRF_ENABLED'] = False
    app.config['DEBUG'] = False
    self.app = app.test_client()

  def test_broken_payload(self):
    payload = {
      "board": "00000000000000000000000000000000000000100000000011000000001100000000111000000001110000001111110000111111110011111111001111111100111111100111101111011111111000111111111011111111101111111110111111111011",
      "piece":"Z_PIECE",
      "next_piece":"J_PIECE",
      "first_move_direction":"RIGHT",
      "line_clears":4
    }
    resp = self.app.post('/get-moves-given-piece', json=payload)
    data = json.loads(resp.get_data(as_text=True))
    demo_entries = data['demo_entries']
    self.assertEqual(str(FIRST_MOVE_FRAME) + ' RIGHT', demo_entries[0])

  def test_broken_payload2(self):
    payload = {
      'board': '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001100001101011000111111110011111111001111111110111111111011111111101111111110111111101111111110111111111011',
      'piece': 'T_PIECE',
      'next_piece': 'Z_PIECE',
      'first_move_direction': 'LEFT',
      'line_clears': 179
    }
    resp = self.app.post('/get-moves-given-piece', json=payload)
    data = json.loads(resp.get_data(as_text=True))
    demo_entries = data['demo_entries']
    self.assertEqual(str(FIRST_MOVE_FRAME) + ' LEFT', demo_entries[0])

if __name__ == '__main__':
  unittest.main()
