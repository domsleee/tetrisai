#include "src/common/BlockType.hpp"
#include <unordered_map>
#include <stdexcept>

BlockType fromString(const std::string &blockStr) {
  static const std::unordered_map<std::string, BlockType> m{{
    {"I_PIECE", BlockType::I_PIECE},
    {"T_PIECE", BlockType::T_PIECE},
    {"J_PIECE", BlockType::J_PIECE},
    {"L_PIECE", BlockType::L_PIECE},
    {"O_PIECE", BlockType::O_PIECE},
    {"S_PIECE", BlockType::S_PIECE},
    {"Z_PIECE", BlockType::Z_PIECE},
  }};

  if (m.count(blockStr)) {
    return m.at(blockStr);
  }
  printf("blockStr: %s\n", blockStr.c_str());
  throw std::runtime_error("unknown blocktype");
}