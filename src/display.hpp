#ifndef DISPLAY_HPP
#define DISPLAY_HPP
// holds configuration for the entire keyboard
#include "config.hpp"
// display
#include <U8g2lib.h>

class Display : public U8G2_SSD1306_128X64_NONAME_F_HW_I2C {
private:
public:
  U8G2LOG log;
  std::vector<uint8_t> log_buffer;
  Display(Config *config);
  void sleep();
  void wakeup();
  // void print(uint8_t x, uint8_t y, text);
};

#endif
