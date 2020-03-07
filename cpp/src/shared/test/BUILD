
cc_library(
  name = "MoveEvaluatorUtility",
  srcs = ["MoveEvaluatorUtility.cpp"],
  hdrs = ["MoveEvaluatorUtility.hpp"],
  visibility = ["//visibility:public"],
  deps = ["//src:board", "//src:shared"]
)

load("//:defs.bzl", "gen_test_rules")

gen_test_rules(
  deps = ["//src:shared", "//:catch", "//src:pso", ":MoveEvaluatorUtility"]
)
