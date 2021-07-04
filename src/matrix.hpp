#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <Arduino.h>
#include <cstddef>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

// allow layout to access matrix
#include "layout.hpp"
// configuration
#include "config.hpp"

#include <iostream>
#include <string>

#include "types.h"

class Matrix {
  // properties
  size_t debounce;
  std::vector<size_t> source_pins;
  std::vector<size_t> sinc_pins;

  bool row2col;
  // holds pin state

  // friend Layout;
  void show_switch(keyswitch_t *key);

  std::unordered_map<size_t, std::unordered_map<size_t, keyswitch_t>> keys;

public:
  // scans the pins
  //
  // std::unordered_map<keyswitch_t, keyswitch_t> active_keys;
  std::vector<keyswitch_t> active_keys;
  void scan();
  void print_ak();
  void update();
  void get_pinmode();
  // determine if switch was activated
  void determine_activity(keyswitch_t *key);
  void setup_pins();
  void setup_keys();
  Matrix(Config *config);
};
#endif