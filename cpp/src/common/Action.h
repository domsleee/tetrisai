#pragma once

#include "src/common/MoveDirection.hpp"
#include "src/common/RotateDirection.hpp"

#include <string>

enum class Action {
  ROTATE_AC = 0,
  ROTATE_C,
  LEFT,
  RIGHT,
  UP,
  DOWN,
  NONE
};
Action toAction(MoveDirection md);
Action toAction(RotateDirection rd);
std::string toString(Action a);

