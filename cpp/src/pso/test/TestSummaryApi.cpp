#include "catch.h"
#include "src/pso/summary/SummaryApi.h"
#include "src/common/common.hpp"

const std::string TEST_DATA_FOLDER = WORKSPACE_DIR + std::string("/src/pso/test/data");

SCENARIO("readLogFile") {
  SummaryApi s(TEST_DATA_FOLDER);
  auto res = s.readLogFile("log");
  auto expGroup = "Linear";
  Config expConfig;
  expConfig.numLines = 230;
  expConfig.maxDropRem = 2;
  expConfig.startingLines = 130;
  expConfig.startingLevel = 19;
  expConfig.averageAmount = 50;
  Weighting expW = WeightingFn::readFromString("[-9.042624222514707 10.103664628141122 7.16493146333065 14.725426784033669 3.943683874407148 8.052072924340575 18.65162441739122 2.0063592045498173 10.037454162384225 1.37738963243647 10.182288334147447 1.56591357159955 -11.92293960546652 -6.770123948180089 -15.844349667168675 1.6356014290763436 0.7199133355753471 -1.8064954184973119 0.7186715483406683 -23.29819536225911]");
  REQUIRE(expConfig.toMap() == res.config.toMap());
  REQUIRE(expGroup == res.group);
  REQUIRE(expW == res.weights);
}

SCENARIO("getAllNames") {
  SummaryApi s(TEST_DATA_FOLDER);
  auto res = s.getAllNames();
  REQUIRE(1 == res.size());
  REQUIRE("log" == res[0]);
}
