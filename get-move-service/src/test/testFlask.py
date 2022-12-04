#!/usr/bin/env python3

import unittest
from main import app
import logging
import json

from testUtility.defs import FIRST_MOVE_FRAME


class TestFlask(unittest.TestCase):
  def setUp(self):
    app.config['TESTING'] = True
    app.config['WTF_CSRF_ENABLED'] = False
    app.config['DEBUG'] = False
    self.app = app.test_client()

  def test_broken_payload(self):
    payload = {
      "board": "0000000000000000000000000000000000000000000000000000000000100000000011000000001100000000111000000001110000001111110000111111110011111111001111111100111111100111101111011111111000111111111011111111101111111110111111111011",
      "piece":"Z_PIECE",
      "next_piece":"J_PIECE",
      "line_clears":4
    }
    resp = self.app.post('/get-moves-given-piece', json=payload)
    data = json.loads(resp.get_data(as_text=True))
    demo_entries = data['demo_entries']

  def test_broken_payload2(self):
    payload = {
      'board': '0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001100001101011000111111110011111111001111111110111111111011111111101111111110111111101111111110111111111011',
      'piece': 'T_PIECE',
      'next_piece': 'Z_PIECE',
      'line_clears': 179
    }
    resp = self.app.post('/get-moves-given-piece', json=payload)
    data = json.loads(resp.get_data(as_text=True))
    demo_entries = data['demo_entries']
    #self.assertGreater(len(demo_entries), 0)
    self.assertEqual(str(FIRST_MOVE_FRAME) + ' DOWN', demo_entries[0])

  def test_broken_payload3(self):
    payload = {
      'board': '0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000011011100001111111000111111110011111111001111111101111111',
      'piece': 'T_PIECE',
      'next_piece': 'J_PIECE',
      'line_clears': 230
    }
    resp = self.app.post('/get-moves-given-piece', json=payload)
    data = json.loads(resp.get_data(as_text=True))
    demo_entries = data['demo_entries']
    #self.assertGreater(len(demo_entries), 0)
    self.assertEqual('1 DOWN', demo_entries[0])

  def test_broken_payload4(self):
    payload = {
      'board': '0000000000000000000000000110000000110000000111100000001010000000111000000111110000001111000000111000000011100000001110000000110000000011000000001111000000111100000011110110001111111000111111100011111110011111111101111111',
      'piece': 'O_PIECE',
      'next_piece': 'I_PIECE',
      'line_clears': 231
    }
    resp = self.app.post('/get-moves-given-piece', json=payload)
    data = json.loads(resp.get_data(as_text=True))
    self.assertEqual(0, len(data.keys()))


if __name__ == '__main__':
  unittest.main()
