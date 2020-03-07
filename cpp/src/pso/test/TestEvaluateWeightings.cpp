#include "src/pso/RunOneGame.h"
#include "src/common/BlockType.hpp"
#include "src/common/Weighting.hpp"
#include <vector>
#include "catch.h"


// class MockGetNextMove: public GetNextMove {
//  public:
//   int calls = 0;
//   Move getNextMove(const SimpleBoard& board, BlockType blockType) override {
//     calls++;
//     return Move({{1,1},{2,2},{3,3},{4,4}});
//   }
// };

SCENARIO("a piece set of 40 pieces finishes") {
  GIVEN("a piece set of 40 pieces") {
    std::vector<BlockType> pieces(40, BlockType::I_PIECE);
    Weighting w;
    RunOneGame r;
    WHEN("we run the game") {
      THEN("it finishes") {
        r.runGame(pieces, w);
        REQUIRE(true);
      }
    }
  }
}
