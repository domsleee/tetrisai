import unittest
from unittest import mock
import os

from tetrisai.interfaces.i_run_particle import IRunParticle
from tetrisai.run_particle_with_data import RunParticleWithData

from tetrisai.test.data.test_bin import board, board2, blockType, blockType2, ids, ids2, score

DIR = os.path.dirname(os.path.abspath(__file__))

class TestRunParticleWithData(unittest.TestCase):
  def setUp(self):
    self._instance = RunParticleWithData(os.path.join(DIR, 'data', 'test_bin.py'))

  def test_correct_score(self):
    my_score = self._instance.run_sync([])
    exp_score = float(score)
    self.assertEqual(exp_score, my_score)
  
  def test_data_is_captured(self):
    self._instance.run_sync([])
    state_to_moves, ct = self._instance.get_moves_and_count()
    self.assertIn((board, blockType), state_to_moves)
    self.assertEqual(ids, state_to_moves[(board, blockType)])
    self.assertIn((board2, blockType2), state_to_moves)
    self.assertEqual(ids2, state_to_moves[(board2, blockType2)])

