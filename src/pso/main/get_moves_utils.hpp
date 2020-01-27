
#pragma once
#include "src/common/Weighting.hpp"
#include "src/common/common.hpp"
#include "src/pso/ClientApi.hpp"
#include "src/shared/MoveFinder/MoveFinderFSM.h"

const int LINE_TRANSITION = 90;
const int LINE_ON_LEVEL_19 = 130;
const int LINE_ON_LEVEL_29 = 230;

std::string best18 = "[-0.9981266671825644 6.314235664122202 9.218841471366666 0.27802110735477736 -4.1780630417598195 7.433743023240332 8.34877823356098 1.8788685887367613 -0.8499180634596748 -1.8078460542586594 7.642165019678991 -4.116700896835317 4.954418944561089 -12.37120499004913 -11.044493568377911 1.8097163771233449 -1.0832040443509103]";
std::string best19 = "[-14.89103037780527 6.1859328293901985 3.1475886283096397 4.549423856563027 6.4739513866334395 7.16934616046338 19.220915906005704 -1.5353264906442918 8.606784031245795 4.760696336241033 13.769672756691616 -2.0002884453295877 -2.4896370025512127 -8.345237667961287 -13.478496677643442 1.0396980163363931 2.178578911705224]";

auto w1 = WeightingFn::readFromString(best18);
auto w2 = WeightingFn::readFromString(best19);

auto getMeMfPair(int num_lines) {
  auto me1 = MoveEvaluatorAdapter(MoveEvaluator(), w1);
  auto me2 = MoveEvaluatorAdapter(MoveEvaluator(), w2);
  
  auto mf1 = MoveFinderFSM();
  auto mf2 = MoveFinderFSM();
  auto mf3 = MoveFinderFSM();
  mf2.setMaxDropRem(2);
  mf3.setMaxDropRem(1);

  if (num_lines >= LINE_ON_LEVEL_29) {
    return std::pair(me2, mf3);
  }
  if (num_lines >= LINE_ON_LEVEL_19) {
    return std::pair(me2, mf2);
  }
  else if (num_lines >= LINE_TRANSITION) {
    return std::pair(me2, mf1);
  }
  return std::pair(me1, mf1);
}
