#include "src/common/Weighting.hpp"
#include "src/common/Debug.hpp"

namespace WeightingFn {
  Weighting readFromStream(std::istream &is, int num_factors) {
    DEBUG_WEIGHTING("Reading from stream...");
    Weighting w(num_factors);
    char tc;
    is >> tc;
    for (int i = 0; i < num_factors; ++i) {
      std::string t;
      is >> t;
      w[i] = atof(t.c_str());
    }
    DEBUG_WEIGHTING("Done!!\n");
    return w;
  }

  Weighting readFromString(const std::string &s, int num_factors) {
    std::stringstream ss(s);
    return readFromStream(ss, num_factors);
  }
};
