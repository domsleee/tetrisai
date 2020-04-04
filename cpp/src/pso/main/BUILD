load("//:defs.bzl", "EXEC_COPTS", "shared_library")

shared_library(
    name = "RowGenerator",
    deps = ["//src:pso", "//src:shared", "//src:common", "//third_party:libfort"]
)


cc_binary(
    name = "ew",
    srcs = ["main.cpp"],
    deps = ["//src:pso"],
    copts = EXEC_COPTS
)

cc_binary(
    name = "ew_get_moves",
    srcs = ["main_get_moves.cpp"],
    deps = ["//src:pso"],
    copts = EXEC_COPTS
)

cc_binary(
    name = "get_transition_evaluation",
    srcs = ["main_get_transition_evaluation.cpp"],
    deps = ["//src:pso"],
    copts = EXEC_COPTS
)

cc_binary(
    name = "bench_time",
    srcs = ["main_bench_time.cpp"],
    deps = ["//src:pso"],
    copts = EXEC_COPTS + ["-DPARALLEL=seq"]
)

cc_binary(
    name = "summary",
    srcs = ["main_summary.cpp"],
    deps = ["//src:pso", "//third_party:libfort", ":RowGenerator"],
    copts = EXEC_COPTS
)


cc_binary(
    name = "run_pieceset",
    srcs = ["main_run_pieceset.cpp"],
    deps = ["//src:pso"],
    copts = EXEC_COPTS
)

