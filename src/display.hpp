#ifndef DISPLAY_HPP
#define DISPLAY_HPP
// holds configuration for the entire keyboard
#include "config.hpp"
// display
#include <U8g2lib.h>

class Display : public U8G2_SSD1306_128X64_NONAME_F_HW_I2C {
private:
  void setPreChargePeriod(uint8_t p1, uint8_t p2);
  void setVComDeselect(uint8_t v);

public:
  U8G2LOG log;
  std::vector<uint8_t> log_buffer;
  Display(Config *config);
  void sleep();
  void wakeup();

  void draw_battery(double battery_level);
  void draw_text(String text, int y);
  void update(double battery_level);
  // void print(uint8_t x, uint8_t y, text);
};

#endif
