#include "src/shared/MoveEvaluator/Evaluators/MoveEvaluatorBlockBothAdvUtility.h"
#include <algorithm>

int getNumBlockedRows(const int* colHeights, const std::array<bool, NUM_COLUMNS> &isColAccessible) {
  int numBlockedRows = 0;
  if (!isColAccessible[5]) numBlockedRows = NUM_ROWS;
  else {
    bool leftOk = false;
    for (int c = 4; c > 0; --c) {
      leftOk = isColAccessible[c];
      if (!leftOk) {
        numBlockedRows += (NUM_ROWS - *std::min_element(colHeights, colHeights+c));
        break;
      }
    }
    bool rightOk = false;
    for (int c = 6; c < NUM_COLUMNS-1; ++c) {
      rightOk = isColAccessible[c];
      if (!rightOk) {
        numBlockedRows += (NUM_ROWS - *std::min_element(colHeights+c, colHeights+NUM_ROWS));
        break;
      }
    }
  }
  return numBlockedRows;
}
