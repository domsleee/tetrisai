#!/usr/bin/env python3
import unittest
from GetMoves import GetMoves
from utility.BitBoard import BitBoard
from common import piece_to_int
import asyncio
import logging
from testUtility.defs import FIRST_MOVE_FRAME


class TestGetMoves(unittest.TestCase):
  FULL_ROW = "1" + "0" + ("1" * 8)
  EMPTY_ROW = "0" * 10

  def setUp(self):
    self.gm = GetMoves()

  def tearDown(self):
    self.gm.close()
  
  def test_no_moves(self):
    board = '1' * 200
    piece = 0
    result = self.gm.get_moves(board, piece)
    self.assertEqual("n", result.result)

  def test_one_move(self):
    FULL_ROW = "1" + "0" + ("1" * 8)
    EMPTY_ROW = "0" * 10
    board = BitBoard.fromBoard([
      "1111001111",
      "1111001111"
    ] + [FULL_ROW for _ in range(18)])
    exp_nx_board = BitBoard.fromBoard([EMPTY_ROW for _ in range(2)] + [FULL_ROW for _ in range(18)])
    piece = piece_to_int("O_PIECE")
    result1 = self.gm.get_moves(board, piece)
    self.assertEqual(exp_nx_board, result1.nx_board)
    self.assertEqual([], result1.demo_entries)
    
    result2 = self.gm.get_moves(board, piece)
    self.assertEqual(exp_nx_board, result2.nx_board)
    self.assertEqual([], result2.demo_entries)

  def test_i_piece(self):
    board = BitBoard.fromBoard(
      ["0000000000" for _ in range(16)]
      + ["1111111110" for _ in range(4)])
    exp_nx_board = BitBoard.fromBoard([self.EMPTY_ROW for _ in range(20)])
    piece = piece_to_int("I_PIECE")
    result = self.gm.get_moves(board, piece)
    self.assertEqual(exp_nx_board, result.nx_board)
    self.assertGreater(len(result.demo_entries), 0)
    self.assertEqual(FIRST_MOVE_FRAME, result.demo_entries[0].frame)
    self.assertEqual("RIGHT", result.demo_entries[0].action)

  def test_i_piece_given_first_move(self):
    board = BitBoard.fromBoard(
      ["0000000000" for _ in range(16)]
      + ["1111111110" for _ in range(4)])
    not_exp_nx_board = BitBoard.fromBoard([self.EMPTY_ROW for _ in range(20)])
    piece = piece_to_int("I_PIECE")
    result = self.gm.get_moves(board, piece, "LEFT")
    self.assertNotEqual(not_exp_nx_board, result.nx_board)
    self.assertGreater(len(result.demo_entries), 0)
    self.assertEqual(FIRST_MOVE_FRAME, result.demo_entries[0].frame)
    self.assertEqual("LEFT", result.demo_entries[0].action)

  def test_set_num_lines(self):
    self.gm.set_num_lines(5)
    self.assertEqual(5, self.gm.get_num_lines())
    self.gm.set_num_lines(8)
    self.assertEqual(8, self.gm.get_num_lines())

  def test_given_next_piece_greedy_tetris(self):
    board = BitBoard.fromBoard(
      ["0000000000" for _ in range(16)]
      + ["1111111110" for _ in range(4)])
    exp_nx_board = BitBoard.fromBoard([self.EMPTY_ROW for _ in range(20)])
    piece = piece_to_int("I_PIECE")
    result = self.gm.get_moves_given_piece(board, piece, piece)
    self.assertEqual(exp_nx_board, result.nx_board)
    self.assertGreater(len(result.demo_entries), 0)
    self.assertEqual(FIRST_MOVE_FRAME, result.demo_entries[0].frame)
  
  def test_given_next_piece_into_tetris(self):
    board = BitBoard.fromBoard(
      ["0000000000" for _ in range(16)]
      + ["1111111000" for _ in range(2)]
      + ["1111111110" for _ in range(2)])
    exp_nx_board = BitBoard.fromBoard(
      ["0000000000" for _ in range(16)]
      + ["1111111110" for _ in range(4)])
    piece1 = piece_to_int("O_PIECE")
    piece2 = piece_to_int("I_PIECE")
    result = self.gm.get_moves_given_piece(board, piece1, piece2)
    self.assertEqual(exp_nx_board, result.nx_board)
    self.assertGreater(len(result.demo_entries), 0)
    self.assertEqual(FIRST_MOVE_FRAME, result.demo_entries[0].frame)


if __name__ == '__main__':
  unittest.main()
