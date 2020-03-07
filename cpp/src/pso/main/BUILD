load("//:defs.bzl", "EXEC_COPTS")

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
