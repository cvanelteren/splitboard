#include "keyswitch.hpp"
#include <Arduino.h>

KeyMessage::KeyMessage(bool pressed_down, uint8_t row, uint8_t col,
                       size_t time) {
  this->pressed_down = pressed_down;
  this->row = row;
  this->col = col;
  this->time = time;
};

KeySwitch::KeySwitch(keyswitch_cfg_t config) {
  this->config = config;
  this->pressed_down = 0;
  this->buffer = 0;
  this->time = 0;
}

bool KeySwitch::debounce() {
  /**
   * @brief      Debounce keys
   *
   * @details  Debouces the  keys,  i.e.  determine a  key's
   * activitiy through multiple time samples. This functions
   * pushes a debounced key into active_scan_keys.
   *
   */

  pinMode(this->config.sinc, INPUT_PULLUP); // set key high
  auto active = !digitalRead(this->config.sinc);
  bool changed = false; // did the state change?
  // fill filter
  this->buffer <<= 1;
  if (active) {
    this->buffer |= 0x01;
  }

  // check if rising edge
  if ((this->buffer == 0xff) && (!this->pressed_down)) {
    this->pressed_down = true;
    this->time = millis();
    changed = true;
  }
  // check falling edge
  else if ((this->buffer == 0x00) && (this->pressed_down)) {
    this->pressed_down = false;
    changed = true;
  }
  pinMode(this->config.sinc, INPUT); // set key low
  return changed;
}

AbstractMessage KeySwitch::create_msg() const {
  return AbstractMessage();
  // return AbstractMess(this->pressed_down, this->config.row this->config.col,
  // this->time);
}
