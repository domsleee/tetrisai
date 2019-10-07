#pragma once
#include "src/common/BlockType.hpp"
#include "src/common/Move.hpp"

struct PositionPieceGetter {
  static Move getStartingPosition(BlockType blockType) {
    switch (blockType) {
      case BlockType::I_PIECE: {
        return {{
          {0, 3}, {0, 4}, {0, 5}, {0, 6}
        }};
      }
      case BlockType::S_PIECE: {
        return {{
          {0, 5}, {0, 6}, {1, 4}, {1, 5}
        }};
      }
      case BlockType::Z_PIECE: {
        return {{
          {0, 4}, {0, 5}, {1, 5}, {1, 6}
        }};
      }
      case BlockType::O_PIECE: {
        return {{
          {0, 4}, {0, 5}, {1, 4}, {1, 5}
        }};
      }
      case BlockType::T_PIECE: {
        return {{
          {0, 4}, {0, 5}, {0, 6}, {1, 5}
        }};
      }
      case BlockType::J_PIECE: {
        return {{
          {0, 4}, {0, 5}, {0, 6}, {1, 6}
        }};
      }
      case BlockType::L_PIECE: {
        return {{
          {0, 4}, {0, 5}, {0, 6}, {1, 4}
        }};
      }
      default: throw new std::runtime_error{"should never happen"};

    }
  }
};