#pragma once
#include <vector>

enum RotateDirection {
  ROTATE_AC=0,
  ROTATE_C,
};

static std::vector<RotateDirection> allRotateDirections = {
  RotateDirection::ROTATE_AC,
  RotateDirection::ROTATE_C
};
