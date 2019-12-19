BENCHMARK_COPTS = ["-O2", "-DNDEBUG"]
EXEC_COPTS = ["-O2"]
EW_MOVE_FINDER_CACHE_COPTS = ["-O2", "-DMOVE_FINDER_CACHE"]

cc_binary(
    name = "tetrisai",
    srcs = glob(["*.cpp", "*.hpp", "interfaces/*"]),
    deps = [ ":common",
        ":shared"
    ],
)

cc_library(
    name = "common",
    srcs = glob(["common/*.cpp"]),
    hdrs = glob(["common/*.hpp", "common/*.tpp"]),
    deps = [],
)

cc_library(
    name = "shared",
    hdrs = glob(["shared/*.h", "shared/*.hpp", "shared/*.tpp", "shared/MoveEvaluator/*.hpp", "shared/MoveFinder/*.h", "shared/MoveFinder/*.hpp", "shared/MoveFinder/*.tpp"]),
    srcs = glob(["shared/*.cpp", "shared/MoveFinder/*.cpp", "shared/MoveEvaluator/*.cpp"]),
    deps = [":common", ":board"]
)

cc_library(
    name = "board",
    hdrs = glob(["board/*.h", "board/*.hpp", "board/*.tpp", "board/bitboard/*.h"]),
    srcs = glob(["board/*.cpp", "board/bitboard/*.cpp"]),
    deps = [":common"]
)

cc_library(
    name = "pso",
    hdrs = glob(["pso/*.h", "pso/*.hpp", "pso/random/*.hpp", "pso/*.tpp"]),
    srcs = glob(["pso/*.cpp"]),
    deps = [":common", ":board", ":shared", "//third_party:optim", "@boost//:random"]
)

cc_library(
    name = "demo",
    hdrs = glob(["demo/*.h", "demo/*.hpp", "demo/*.tpp"]),
    srcs = glob(["demo/*.cpp"]),
    deps = [":common", ":board", ":shared", ":pso"]
)

cc_binary(
    name = "run_demo",
    deps = [":demo"]
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
    name = "test_move_finder_rewrite",
    srcs = ["shared/test/TestMoveFinderRewrite.cpp"],
    deps = [":shared", "//:catch"]
)

cc_test(
    name = "test_all_move_finder",
    srcs = ["shared/test/TestAllMoveFinder.cpp"],
    deps = [":shared", "//:catch"]
)

cc_test(
    name = "test_move_evaluator",
    srcs = glob(["shared/test/TestMoveEvaluator.cpp", "shared/test/*.hpp", "shared/test/MoveEvaluatorUtility.cpp"]),
    deps = [":shared", "//:catch"]
)

cc_test(
    name = "test_move_evaluator_penalty",
    srcs = glob(["shared/test/TestMoveEvaluatorPenalty.cpp", "shared/test/*.hpp", "shared/test/*.h", "shared/test/MoveEvaluatorUtility.cpp"]),
    deps = [":shared", "//:catch"]
)

cc_binary(
    name = "ew",
    srcs = ["pso/main/main.cpp"],
    deps = [":pso"],
    copts = EXEC_COPTS
)

cc_binary(
    name = "ew_bench",
    srcs = ["pso/main/main_bench.cpp"],
    deps = [":pso"],
    copts = EXEC_COPTS
)

cc_binary(
    name = "ew_str",
    srcs = ["pso/main/main_str.cpp"],
    deps = [":pso"],
    copts = EXEC_COPTS
)

cc_binary(
    name = "ew_18_19",
    srcs = ["pso/main/main_18_19.cpp"],
    deps = [":pso"],
    copts = EXEC_COPTS
)

cc_binary(
    name = "precompute",
    srcs = ["pso/main/main_precompute.cpp"],
    deps = [":pso"],
    copts = EXEC_COPTS
)
cc_binary(
    name = "ew_move_finder_cache",
    srcs = ["pso/main/main.cpp", "shared/MoveFinder/debug/debug.cpp"],
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
    name = "output_piece_sets",
    srcs = ["pso/bench/outputPieceSets.cpp"],
    deps = [":pso"],
    copts = BENCHMARK_COPTS
)
