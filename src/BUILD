BENCHMARK_COPTS = ["-O2", "-DNDEBUG"]
EXEC_COPTS = ["-O2"]
EW_MOVE_FINDER_CACHE_COPTS = ["-O2", "-DMOVE_FINDER_CACHE"]

cc_library(
    name = "common",
    srcs = glob(["common/*.cpp"]),
    hdrs = glob(["common/*.hpp", "common/*.tpp"]),
    deps = [],
)

cc_library(
    name = "shared",
    hdrs = glob(["shared/*.h", "shared/*.hpp", "shared/*.tpp", "shared/MoveEvaluator/*.hpp", "shared/MoveFinder/*.h", "shared/MoveFinder/*.hpp", "shared/MoveFinder/*.tpp", "shared/MoveFinder/**/*.h"]),
    srcs = glob(["shared/*.cpp", "shared/MoveFinder/*.cpp", "shared/MoveEvaluator/*.cpp", "shared/MoveFinder/**/*.cpp", "shared/MoveFinder/**/*.hpp"]),
    deps = [":common", ":board"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "board",
    hdrs = glob(["board/*.h", "board/*.hpp", "board/*.tpp", "board/bitboard/*.h", "board/bitboard/*.hpp"]),
    srcs = glob(["board/*.cpp", "board/bitboard/*.cpp"]),
    deps = [":common"]
)

cc_library(
    name = "pso",
    hdrs = glob(["pso/*.h", "pso/*.hpp", "pso/random/*.hpp", "pso/*.tpp", "pso/main/*.hpp"]),
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
    deps = [":shared", "//:catch", ":move_evaluator_utility"]
)

cc_test(
    name = "test_move_finder_rewrite",
    srcs = ["shared/test/TestMoveFinderRewrite.cpp"],
    deps = [":shared", "//:catch"   ]
)

cc_test(
    name = "test_all_move_finder",
    srcs = ["shared/test/TestAllMoveFinder.cpp"],
    deps = [":shared", "//:catch"]
)

cc_test(
    name = "test_move_evaluator",
    srcs = glob(["shared/test/TestMoveEvaluator.cpp", "shared/test/*.hpp", "shared/test/MoveEvaluatorUtility.cpp"]),
    deps = [":shared", "//:catch", ":pso"]
)

cc_test(
    name = "test_move_evaluator_penalty",
    srcs = glob(["shared/test/TestMoveEvaluatorPenalty.cpp", "shared/test/*.hpp", "shared/test/*.h", "shared/test/MoveEvaluatorUtility.cpp"]),
    deps = [":shared", "//:catch"]
)

cc_test(
    name = "test_move_evaluator_tetris_ready",
    srcs = glob(["shared/test/TestMoveEvaluatorTetrisReady.cpp", "shared/test/*.hpp", "shared/test/*.h", "shared/test/MoveEvaluatorUtility.cpp"]),
    deps = [":shared", "//:catch"]
)

cc_test(
    name = "test_move_evaluator_block_linear_no_tetris_ready",
    srcs = glob(["shared/test/TestMoveEvaluatorBlockLinearNoTetrisReady.cpp", "shared/test/*.hpp", "shared/test/*.h", "shared/test/MoveEvaluatorUtility.cpp"]),
    deps = [":shared", "//:catch"]
)

cc_test(
    name = "test_move_evaluator_block_utility",
    srcs = glob(["shared/test/TestMoveEvaluatorBlockUtility.cpp", "shared/test/*.hpp", "shared/test/*.h", "shared/test/MoveEvaluatorUtility.cpp"]),
    deps = [":shared", "//:catch"]
)

cc_test(
    name = "test_move_evaluator_block",
    srcs = glob(["shared/test/TestMoveEvaluatorBlock.cpp", "shared/test/*.hpp", "shared/test/*.h", "shared/test/MoveEvaluatorUtility.cpp"]),
    deps = [":shared", "//:catch"]
)

cc_test(
    name = "test_move_finder_state",
    srcs = glob(["shared/test/MoveFinderFSM/TestMoveFinderState.cpp"]),
    deps = ["//src:shared", "//:catch"]
)

cc_test(
    name = "test_move_finder_fsm",
    srcs = glob(["shared/test/MoveFinderFSM/TestMoveFinderFSM.cpp"]),
    deps = ["//src:shared", "//:catch", "//src:move_evaluator_utility"]
)

cc_library(
    name = "move_evaluator_utility",
    srcs = glob(["shared/test/MoveEvaluatorUtility.cpp"]),
    hdrs = glob(["shared/test/MoveEvaluatorUtility.hpp"]),
    deps = [":shared"]
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
    name = "ew_get_moves",
    srcs = ["pso/main/main_get_moves.cpp"],
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
    name = "get_transition_evaluation",
    srcs = ["pso/main/main_get_transition_evaluation.cpp"],
    deps = [":pso"],
    copts = EXEC_COPTS
)

cc_binary(
    name = "bench_time",
    srcs = ["pso/main/main_bench_time.cpp"],
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
