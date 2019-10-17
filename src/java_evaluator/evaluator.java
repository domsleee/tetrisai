package java_evaluator;
import java.util.*;
import static java.lang.Math.*;

class Evaluator {
  final static int WIDTH = 10;
  final static int HEIGHT = 20;

  final static int NUM_WEIGHTS = 17;

  // these are members because?
  private int x1;
  private int y1;
  private int o1;
  
  private boolean bestFound;
  private double bestScore;
  private int bestX;
  private int bestY;
  private int bestO;  

  private final int[] columnMinY = new int[WIDTH];

  public double evaluate(int linesCleared, int lockHeight, boolean [][] playfield2, double [] weights) {
    
    final int totalLinesCleared = linesCleared;
    
    final int totalLockHeight = lockHeight;
    
    outer: for(int x = WIDTH - 1; x >= 0; --x) {
      for(int y = 0; y < HEIGHT; ++y) {
        if (playfield2[y][x]) {
          columnMinY[x] = y;
          continue outer;
        }
      }
      columnMinY[x] = HEIGHT;
    }
    
    int maxColumnY = 0;
    int minColumnY = HEIGHT;
    int totalColumnHeights = 0;    
    for(int x = WIDTH - 1; x >= 0; --x) {
      if (columnMinY[x] < minColumnY) {
        minColumnY = columnMinY[x];
      }
      if (columnMinY[x] > maxColumnY) {
        maxColumnY = columnMinY[x];
      }
      totalColumnHeights += HEIGHT - columnMinY[x];
    }
    int columnHeightSpread = maxColumnY - minColumnY;
    
    int totalWellCells = 0;
    int totalDeepWells = 0;
    for(int x = WIDTH - 1; x >= 0; --x) {
      int wellCells = 0;
      switch(x) {
        case 0: {
          final int minY = columnMinY[x + 1];
          for(int y = columnMinY[x] - 1; y >= minY; --y) {
            if (playfield2[y][x + 1]) {
              ++wellCells;
            } 
          }
          break;
        }
        case WIDTH - 1: {
          final int minY = columnMinY[x - 1];
          for(int y = columnMinY[x] - 1; y >= minY; --y) {
            if (playfield2[y][x - 1]) {
              ++wellCells;
            }
          }
          break;
        }
        default: {
          final int minY = min(columnMinY[x - 1], columnMinY[x + 1]);
          for(int y = columnMinY[x] - 1; y >= minY; --y) {
            if (playfield2[y][x - 1] && playfield2[y][x + 1]) {
              ++wellCells;
            } 
          }
          break;
        }
      }
      totalWellCells += wellCells;
      if (wellCells >= 3) {
        ++totalDeepWells;
      }
    }
    
    int totalColumnHoles = 0;
    int totalWeightedColumnHoles = 0;
    int totalColumnTransitions = 0;
    int totalColumnHoleDepths = 0;
    int minColumnHoleDepth = HEIGHT;
    int maxColumnHoleDepth = 0;
    for(int x = WIDTH - 1; x >= 0; --x) {
      final int minY = columnMinY[x] + 1;
      for(int y = HEIGHT - 1; y >= minY; --y) {
        final boolean oy = playfield2[y][x];
        final boolean my = playfield2[y - 1][x];
        if (!oy && my) {
          ++totalColumnHoles;
          totalWeightedColumnHoles += y + 1;
          final int depth = y - columnMinY[x];
          totalColumnHoleDepths += depth;
          if (depth < minColumnHoleDepth) {
            minColumnHoleDepth = depth;
          }
          if (depth > maxColumnHoleDepth) {
            maxColumnHoleDepth = depth;
          }
        }
        if (oy != my) {
          ++totalColumnTransitions;
        }
      }
    }
    
    int totalRowTransitions = 0;
    for(int y = HEIGHT - 1; y >= minColumnY; --y) {
      final boolean[] row = playfield2[y];
      if (!row[0]) {
        ++totalRowTransitions;
      }
      if (!row[WIDTH - 1]) {
        ++totalRowTransitions;
      }
      for(int x = WIDTH - 1; x >= 1; --x) {
        if (row[x] != row[x - 1]) {
          ++totalRowTransitions;
        }
      }
    }
    
    final int pileHeight = HEIGHT - minColumnY;
    
    int totalSolidCells = 0;
    int totalWeightedSolidCells = 0;
    for(int x = WIDTH - 1; x >= 0; --x) {
      final int minY = columnMinY[x];
      for(int y = HEIGHT - 1; y >= minY; --y) {
        if (playfield2[y][x]) {
          ++totalSolidCells;
          totalWeightedSolidCells += HEIGHT - y;
        }
      }
    }
    
    int columnHeightVariance = 0;
    for(int x = WIDTH - 1; x >= 1; --x) {
      columnHeightVariance += abs(columnMinY[x] - columnMinY[x - 1]);
    }
    
    double score
        = weights[0]  * totalLinesCleared
        + weights[1]  * totalLockHeight
        + weights[2]  * totalWellCells
        + weights[3]  * totalDeepWells
        + weights[4]  * totalColumnHoles
        + weights[5]  * totalWeightedColumnHoles
        + weights[6]  * totalColumnHoleDepths
        + weights[7]  * minColumnHoleDepth
        + weights[8]  * maxColumnHoleDepth
        + weights[9]  * totalColumnTransitions
        + weights[10] * totalRowTransitions
        + weights[11] * totalColumnHeights
        + weights[12] * pileHeight
        + weights[13] * columnHeightSpread
        + weights[14] * totalSolidCells
        + weights[15] * totalWeightedSolidCells
        + weights[16] * columnHeightVariance;
        
    for(int y = 6; y >= minColumnY; --y) {
      for(int x = WIDTH - 1; x >= 0; --x) {
        if (playfield2[y][x]) {
          score += 7 - y;
        }
      }
    }
    
    if (linesCleared == 4) {
      score -= 1.0E9;
    }
    
    if (score < bestScore) {
      bestFound = true;
      bestScore = score;
      bestX = x1;
      bestY = y1;
      bestO = o1;
    }

    return score;
  }
}

