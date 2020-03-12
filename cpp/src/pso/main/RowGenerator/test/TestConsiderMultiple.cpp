
#include "catch.h"
#include "src/pso/main/RowGenerator/rows/AllRows.h"
#include <memory>

SCENARIO("ConsiderMultiple polymorphism") {
  auto nameRow = std::make_shared<Name>();
  auto groupRow = std::make_shared<Group>();

  REQUIRE(false == nameRow->considerMultiple());
  REQUIRE(true == groupRow->considerMultiple());
}
