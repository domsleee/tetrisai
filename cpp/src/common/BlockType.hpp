#pragma once
#include <vector>
#include <string>

enum BlockType {
  I_PIECE=0,
  L_PIECE,
  J_PIECE,
  T_PIECE,
  Z_PIECE,
  S_PIECE,
  O_PIECE
};

static const std::vector<BlockType> allBlockTypes = {
  BlockType::I_PIECE,
  BlockType::L_PIECE,
  BlockType::J_PIECE,
  BlockType::T_PIECE,
  BlockType::Z_PIECE,
  BlockType::S_PIECE,
  BlockType::O_PIECE
};

BlockType fromString(const std::string &blockStr);

// todo
static const int NUM_BLOCK_TYPES = 7;