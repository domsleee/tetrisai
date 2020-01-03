#!/usr/bin/env python3
import unittest
from GetMoves import GetMoves

class TestGetMoves(unittest.TestCase):
  def setUp(self):
    self.gm = GetMoves()

  def tearDown(self):
    self.gm.close()
  
  def test_no_moves(self):
    board = '1' * 200
    piece = 0
    nx_board, demo_entries = self.gm.get_moves(board, piece)
    self.assertEqual(board, nx_board)
    self.assertEqual([], demo_entries)

  def test_one_move(self):
    pass


if __name__ == '__main__':
  unittest.main()