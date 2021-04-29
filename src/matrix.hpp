#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <Arduino.h>
#include <cstddef>
#include <stdexcept>
#include <unordered_map>
#include <utility>

// allow layout to access matrix
#include "layout.hpp"
// configuration
#include "config.hpp"

struct hash_pair {
  template <class T1, class T2>
  size_t operator()(const std::pair<T1, T2> &p) const {
    auto hash1 = std::hash<T1>{}(p.first);
    auto hash2 = std::hash<T2>{}(p.second);
    return hash1 ^ hash2;
  }
};

typedef std::pair<size_t, size_t> switch_t;
class Matrix {
  // scans the pins
  void scan();
  // determine if switch was activated
  void determine_activity(switch_t *pair);

  // properties
  size_t debounce;
  std::vector<size_t> row_pins;
  std::vector<size_t> col_pins;
  // holds pin state
  std::unordered_map<switch_t, size_t, hash_pair> active_keys;

  friend Layout;

public:
  Matrix(Config *config);
};
#endif
