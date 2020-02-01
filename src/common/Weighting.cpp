#include "src/common/Weighting.hpp"
#include "src/common/Debug.hpp"

namespace WeightingFn {
  Weighting readFromStream(std::istream &is) {
    DEBUG_WEIGHTING("Reading from stream...");
    Weighting w;
    char tc;
    is >> tc;
    assert(tc == '[');

    bool lastChar = false;
    std::string s;
    while (is >> s) {
      w.push_back(std::atof(s.c_str()));
      if (s[s.size()-1] == ']') { lastChar = true; break; };
    }

    if (!lastChar) throw std::runtime_error("Failed to read last char");
    return w;
  }

  Weighting readFromString(const std::string &s) {
    std::stringstream ss(s);
    return readFromStream(ss);
  }

  /*
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
  }*/

  
};
