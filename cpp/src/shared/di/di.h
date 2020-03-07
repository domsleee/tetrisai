#pragma once
#include <memory>
#include "src/shared/MoveRecorder/IMoveRecorder.h"

struct Di {
  template<typename MyMoveRecorder>
  static void setMoveRecorder(MyMoveRecorder instance) {
    moveRecorder_ = std::make_unique<MyMoveRecorder>(instance);
  }

  static IMoveRecorder& getMoveRecorder() {
    if (moveRecorder_ == nullptr) {
      throw std::runtime_error{"getMoveRecorder was a nullptr"};
    }
    return *moveRecorder_;
  }

 private:
  static std::unique_ptr<IMoveRecorder> moveRecorder_;
};

