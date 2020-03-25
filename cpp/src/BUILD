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
    deps = [":common", "//src:board", ":shared", "//third_party:optim", "@boost//:random", "@boost//:algorithm"],
)

shared_library(
    name = "board",
    deps = [":common"],
)

