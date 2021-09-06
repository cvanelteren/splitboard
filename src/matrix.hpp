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

#include "types.hpp"

class Matrix {
  // properties
  std::vector<uint8_t> source_pins;
  std::vector<uint8_t> sinc_pins;

  bool row2col;
  // holds pin state

  // friend Layout;
  void show_switch(keyswitch_t *key);

  void add_key(keyswitch_t &key);
  void remove_key(uint8_t idx);

  std::unordered_map<uint8_t, std::unordered_map<uint8_t, keyswitch_t>> keys;

public:
  // scans the pins
  //
  // std::unordered_map<keyswitch_t, keyswitch_t> active_keys;
  //
  Matrix(Config *config);
  std::vector<keyswitch_t> active_keys;
  void scan();
  void print_ak();
  void update();
  void get_pinmode();
  // determine if switch was activated
  void determine_activity(keyswitch_t *key);
  void setup_pins();
  void setup_keys();

  uint8_t get_cols();
  uint8_t get_rows();

  std::vector<uint8_t> get_source_pins();
  std::vector<uint8_t> get_sinc_pins();
  void sleep();
  void wakeup();

#ifdef UNIT_TEST
  friend void test_zero_state_keys();
  friend void test_source_pin_mode();
  friend void test_sinc_pin_mode();
  friend void test_ghosting();
  friend uint8_t number_of_keys_pressed();
#endif
};
#endif
