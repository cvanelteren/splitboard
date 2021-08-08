#ifndef ROTARY_ENCODER_H_
#define ROTARY_ENCODER_H_

#include "config.hpp"
#include <BleKeyboard.h>

class RotaryEncoder {
  Config *config;

  int last_count;
  bool a_state;
  bool b_state;
  size_t last_active_time;

  // uint8_t prevNextCode;
  // uint16_t store;
  int8_t counter; // hold the state of the rotary

  // filter out debounce
  int8_t read_rotary();
  const bool enc_table[16]{0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0};
  uint8_t state;
  uint16_t filter;

  std::vector<int8_t> active_keys;

public:
  RotaryEncoder(Config *config);
  void onButtonClick();
  void begin();
  void update();
  std::vector<int8_t> get_keys();
};

#endif // ROTARY_ENCODER_H_
