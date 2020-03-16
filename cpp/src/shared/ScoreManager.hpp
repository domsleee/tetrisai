#pragma once
#include <algorithm>

struct ScoreManager {
  ScoreManager() {}
  ScoreManager(int level): level_(level) {
    updateLevel();
  }

  void addLineClears(int lineClears) {
    totalLines_ += lineClears;
    updateLevel();
    switch(lineClears) {
      case 0: break;
      case 1: { score_ += 40 * (level_+1); } break;
      case 2: { score_ += 100 * (level_+1); } break;
      case 3: { score_ += 300 * (level_+1); } break;
      case 4: { score_ += 1200 * (level_+1); } break;
    }
  }
  int getScore() const {
    return score_;
  }
  int getLevel() const {
    return level_;
  }
  int getTotalLines() const {
    return totalLines_;
  }
  void setLines(int totalLines) {
    totalLines_ = totalLines;
    updateLevel();
  }
 private:
  // 100 => 16
  // 110 => 17
  // 120 => 18
  // 130 => 19
  void updateLevel() {
    if (totalLines_ >= 130) {
      level_ = 19 + (totalLines_ - 130)/10;
    } else {
      level_ = std::max(level_, 18);
    }
  }

  int level_ = 18;
  int score_ = 0;
  int totalLines_ = 0;
};