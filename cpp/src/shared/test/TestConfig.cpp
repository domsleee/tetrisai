
#include "catch.h"
#include "src/shared/Config.hpp"

SCENARIO("config") {
  for (auto field: Config::allFields) {
    Config cfg;
    cfg.setField(field, "5");
    Config cfg2;
    cfg2.fromMap(cfg.toMap());
    REQUIRE(5 == cfg2.toMap().at(field));
  }
}
