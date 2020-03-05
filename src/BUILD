BENCHMARK_COPTS = ["-O2", "-DNDEBUG"]
EXEC_COPTS = ["-O2", "-DNDEBUG"]
EW_MOVE_FINDER_CACHE_COPTS = ["-O2", "-DMOVE_FINDER_CACHE"]

load("//:defs.bzl", "shared_library")

shared_library(
    name = "common"
)

shared_library(
    name = "shared",
    deps = [":common", "//src:board"]
)

shared_library(
    name = "pso",
    deps = [":common", "//src:board", ":shared", "//third_party:optim", "@boost//:random"],
)

shared_library(
    name = "board",
    deps = [":common"],
)

