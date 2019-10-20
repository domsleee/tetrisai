#pragma once

struct ScoreManager {
  void addLineClears(int lineClears) {
    switch(lineClears) {
      case 0: break;
      case 1: { score_ += 40 * (level_+1); } break;
      case 2: { score_ += 100 * (level_+1); } break;
      case 3: { score_ += 300 * (level_+1); } break;
      case 4: { score_ += 1200 * (level_+1); } break;
    }
    totalLines_ += lineClears;
    if (totalLines_ >= 140) {
      level_ = 19 + (totalLines_ - 130)/10;
    }
  }
  int getScore() const {
    return score_;
  }
  int getTotalLines() const {
    return totalLines_;
  }
 private:
  int level_ = 19;
  int score_ = 0;
  int totalLines_ = 0;
};