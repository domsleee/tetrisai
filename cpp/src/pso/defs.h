#pragma once
#include <string>
#include "src/common/common.hpp"

#define eprintf(...) fprintf(stderr, __VA_ARGS__)
const std::string DATA_FOLDER = WORKSPACE_DIR + std::string("/../python/named_logs");
