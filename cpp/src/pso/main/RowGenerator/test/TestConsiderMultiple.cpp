
#include "catch.h"
#include "src/pso/main/RowGenerator/rows/AllRows.h"
#include <memory>

SCENARIO("ConsiderMultiple polymorphism") {
  auto nameRow = std::make_shared<Name>();
  auto maxoutRateRow = std::make_shared<MaxoutRate>();

  REQUIRE(false == nameRow->considerMultiple());
  REQUIRE(true == maxoutRateRow->considerMultiple());
}
