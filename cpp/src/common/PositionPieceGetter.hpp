#pragma once
#include "src/common/BlockType.hpp"
#include "src/common/Move.hpp"

struct PositionPieceGetter {
  static Move getStartingPosition(BlockType blockType) {
    switch (blockType) {
      case BlockType::I_PIECE: {
        return {{
          {2, 3}, {2, 4}, {2, 5}, {2, 6}
        }};
      }
      case BlockType::S_PIECE: {
        return {{
          {2, 5}, {2, 6}, {3, 4}, {3, 5}
        }};
      }
      case BlockType::Z_PIECE: {
        return {{
          {2, 4}, {2, 5}, {3, 5}, {3, 6}
        }};
      }
      case BlockType::O_PIECE: {
        return {{
          {2, 4}, {2, 5}, {3, 4}, {3, 5}
        }};
      }
      case BlockType::T_PIECE: {
        return {{
          {2, 4}, {2, 5}, {2, 6}, {3, 5}
        }};
      }
      case BlockType::J_PIECE: {
        return {{
          {2, 4}, {2, 5}, {2, 6}, {3, 6}
        }};
      }
      case BlockType::L_PIECE: {
        return {{
          {2, 4}, {2, 5}, {2, 6}, {3, 4}
        }};
      }
      default: throw std::runtime_error{"should never happen"};

    }
  }
};