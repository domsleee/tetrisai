cc_library(
    name = "catch",
    srcs = ["catch.hpp"],
    visibility = ["//:__pkg__"],
    deps = [],
)

cc_library(
    name = "optim",
    srcs = glob(["optim/header_only_version/**/*.hpp", "optim/header_only_version/**/*.ipp", "optim/header_only_version/**/*.h"]),
    visibility = ["//visibility:public"],
    deps = []
)
