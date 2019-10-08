#pragma once

enum MoveDirection {
  LEFT=0,
  RIGHT,
  UP,
  DOWN
};

static std::vector<MoveDirection> allMoveDirections = {
  MoveDirection::DOWN,
  MoveDirection::UP,
  MoveDirection::LEFT,
  MoveDirection::RIGHT
};

static std::vector<MoveDirection> validMoveDirections = {
  MoveDirection::DOWN,
  MoveDirection::LEFT,
  MoveDirection::RIGHT
};

