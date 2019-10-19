BENCHMARK_COPTS = ["-O2", "-DNDEBUG"]

cc_binary(
    name = "tetrisai",
    srcs = glob(["*.cpp", "*.hpp", "interfaces/*"]),
    deps = [
        ":common",
        ":shared"
    ],
)

cc_library(
    name = "common",
    srcs = glob(["common/*.hpp"]),
    deps = [],
)

cc_library(
    name = "shared",
    hdrs = glob(["shared/*.h", "shared/*.hpp", "shared/*.tpp"]),
    srcs = glob(["shared/*.cpp"]),
    deps = [":common", ":board"]
)

cc_library(
    name = "board",
    hdrs = glob(["board/*.h", "board/*.hpp", "board/bitboard/*.h"]),
    srcs = glob(["board/*.cpp", "board/bitboard/*.cpp"]),
    deps = [":common"]
)

cc_library(
    name = "pso",
    hdrs = glob(["pso/*.h", "pso/*.hpp"]),
    srcs = glob(["pso/*.cpp"]),
    deps = [":common", ":board", ":shared", "//third_party:optim"]
)

cc_test(
    name = "board_test",
    srcs = ["board/test/TestBoard.cpp"],
    deps = [
        ":board",
        "//:catch",
        ":common"
    ]
)
cc_test(
    name = "test_bit_board",
    srcs = ["board/test/TestBitBoard.cpp"],
    deps = [
        ":board",
        "//:catch",
        ":common"
    ]
)

cc_test(
    name = "test_move_finder",
    srcs = ["shared/test/TestMoveFinder.cpp"],
    deps = [":shared", "//:catch"]
)

cc_test(
    name = "test_all_move_finder",
    srcs = ["shared/test/TestAllMoveFinder.cpp"],
    deps = [":shared", "//:catch"]
)

cc_test(
    name = "test_move_evaluator",
    srcs = ["shared/test/TestMoveEvaluator.cpp"],
    deps = [":shared", "//:catch"]
)

cc_binary(
    name = "ew",
    srcs = ["pso/main.cpp"],
    deps = [":pso"],
    copts = BENCHMARK_COPTS
)

cc_binary(
    name = "bench_move_finder",
    srcs = ["shared/bench/benchmark.cpp"],
    deps = [":shared", ":common", ":board"],
    copts = BENCHMARK_COPTS
)

cc_binary(
    name = "bench_evaluate_weightings",
    srcs = ["pso/bench/benchEvaluateWeightings.cpp"],
    deps = [":pso"],
    copts = BENCHMARK_COPTS
)
