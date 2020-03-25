#include "src/shared/MoveEvaluator/Evaluators/MoveEvaluatorPenalty.hpp"

// todo: issue when framesPerDrop is not a factor of DAS_FRAMES_PER_MOVE
const int DAS_FRAMES_PER_MOVE = 6;
const int I_PIECE_START_HEIGHT = 18;
/**
 * For level 19 (framesPerDrop = 2)
 * 5: 18
 * 4: 15
 * 3: 12
 * 2: 9
 * 1: 6
 * 0: 3
 */
int reachableHeight(int c, int framesPerDrop) {
  if (c == 5) return 18;
  if (c < 5) {
    return I_PIECE_START_HEIGHT - ((DAS_FRAMES_PER_MOVE/framesPerDrop) * (5-c));
  } else { // c > 6
    return I_PIECE_START_HEIGHT - ((DAS_FRAMES_PER_MOVE/framesPerDrop) * (c-5));
  }
}

