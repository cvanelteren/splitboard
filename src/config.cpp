#include "config.hpp"
// poor man's config file
Config::Config() {
  // define pins
  this->row_pins = {1, 2, 3, 4, 5};
  this->col_pins = {1, 2, 3, 4};

  // define debounce
  this->debounce = 300; // miliseconds
  this->name = "SplitBoard";

  // display settings
  this->pin_sda = GPIO_NUM_4;
  this->pin_scl = GPIO_NUM_15;
  this->pin_reset = GPIO_NUM_16;

  this->display_rotation = U8G2_R0;
}
