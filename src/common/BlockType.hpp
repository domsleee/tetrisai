#pragma once
#include <vector>

enum BlockType {
  I_PIECE,
  L_PIECE,
  J_PIECE,
  T_PIECE,
  Z_PIECE,
  S_PIECE,
  O_PIECE
};

static std::vector<BlockType> allBlockTypes = {
  BlockType::I_PIECE,
  BlockType::L_PIECE,
  BlockType::J_PIECE,
  BlockType::T_PIECE,
  BlockType::Z_PIECE,
  BlockType::S_PIECE,
  BlockType::O_PIECE
};