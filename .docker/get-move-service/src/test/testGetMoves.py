#!/usr/bin/env python3
import unittest
from GetMoves import GetMoves
from utility.BitBoard import BitBoard
from common import piece_to_int
import asyncio


class TestGetMoves(unittest.TestCase):
  def setUp(self):
    self.gm = GetMoves()
    asyncio.get_event_loop().run_until_complete(self.gm.init())

  def tearDown(self):
    self.gm.close()
  
  def test_no_moves(self):
    board = '1' * 200
    piece = 0
    res, nx_board, demo_entries = self.gm.get_moves(board, piece)
    self.assertEqual("n", res)

  def test_one_move(self):
    FULL_ROW = "1" + "0" + ("1" * 8)
    EMPTY_ROW = "0" * 10
    board = BitBoard.fromBoard([
      "1111001111",
      "1111001111"
    ] + [FULL_ROW for _ in range(18)])
    exp_nx_board = BitBoard.fromBoard([EMPTY_ROW for _ in range(2)] + [FULL_ROW for _ in range(18)])
    piece = piece_to_int("O_PIECE")
    _, nx_board, demo_entries = self.gm.get_moves(board, piece)
    self.assertEqual(exp_nx_board, nx_board)
    self.assertEqual(demo_entries, [])
    
    _, nx_board, demo_entries = self.gm.get_moves(board, piece)
    self.assertEqual(exp_nx_board, nx_board)
    self.assertEqual(demo_entries, [])


if __name__ == '__main__':
  unittest.main()
