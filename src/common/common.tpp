#pragma once
#include <algorithm>
#include <numeric>


template<typename T>
double average(T it1, T it2) {
  return (double)std::accumulate(it1, it2, 0) / (it2-it1);
}