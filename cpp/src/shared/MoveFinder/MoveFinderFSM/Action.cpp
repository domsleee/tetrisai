
#include "src/shared/MoveFinder/MoveFinderFSM/Action.h"
#include <stdexcept>

Action toAction(MoveDirection md) {
  switch(md) {
    case MoveDirection::LEFT: return Action::LEFT;
    case MoveDirection::RIGHT: return Action::RIGHT;
    case MoveDirection::UP: return Action::UP;
    case MoveDirection::DOWN: return Action::DOWN;
  }
  throw std::runtime_error("unable to convert move to action");
}

Action toAction(RotateDirection rd) {
  switch(rd) {
    case RotateDirection::ROTATE_AC: return Action::ROTATE_AC;
    case RotateDirection::ROTATE_C: return Action::ROTATE_C;
  }
  throw std::runtime_error("unable to convert rotation to action");
}

std::string toString(Action a) {
  switch(a) {
    case Action::ROTATE_AC: return "ROTATE_AC"; break;
    case Action::ROTATE_C: return "ROTATE_C"; break;
    case Action::LEFT: return "LEFT"; break;
    case Action::RIGHT: return "RIGHT"; break;
    case Action::UP: return "UP"; break;
    case Action::DOWN: return "DOWN"; break;
  }
  throw std::runtime_error("unable to convert action to string");
}