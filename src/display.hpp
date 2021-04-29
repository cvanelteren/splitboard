#ifndef DISPLAY_HPP
#define DISPLAY_HPP
// holds configuration for the entire keyboard
#include "config.hpp"
// display
#include <U8g2lib.h>

class Display : public U8G2_SSD1306_128X64_NONAME_1_SW_I2C {

public:
  Display(Config *config);
  // void print(uint8_t x, uint8_t y, text);
};

#endif
