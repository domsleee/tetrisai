#pragma once

#include <vector>
#include <array>

enum MoveDirection {
  LEFT=0,
  RIGHT,
  DOWN,
  UP
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

static constexpr std::array cooldownMoveDirections = {
  MoveDirection::DOWN,
  MoveDirection::LEFT,
  MoveDirection::RIGHT
};

static std::vector<MoveDirection> sidewaysMoveDirections = {
  MoveDirection::LEFT,
  MoveDirection::RIGHT
};

