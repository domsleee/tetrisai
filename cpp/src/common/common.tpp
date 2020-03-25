#pragma once
#include <algorithm>
#include <numeric>
#include <chrono>


template<typename T>
double average(T it1, T it2) {
  return (double)std::accumulate(it1, it2, 0) / (it2-it1);
}

using Ms = std::chrono::duration<double>;
using Seconds = std::chrono::duration<double>;

template <typename B>
long getMs(B s1) {
  return std::chrono::duration<double, std::milli>(s1).count();
}