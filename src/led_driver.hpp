#ifndef LED_HPP
#define LED_HPP

#include "config.hpp"
#include <FastLED.h>
#include <cstdint>
class LED {
  uint8_t num_leds;
  uint8_t led_pin;
  uint8_t brightness;
  CRGB *leds;

private:
  std::vector<uint8_t> led_col_bins;

public:
  void begin();
  LED(Config *Config);
  ~LED();
  void cycle();
  void set_color(uint8_t hue, uint8_t saturation, uint8_t value);

  // drive specific locations
  std::pair<uint8_t, uint8_t> int2grid(uint8_t idx);
  uint8_t grid2int(std::pair<uint8_t, uint8_t> grid);
  void sleep();
  void wakeup();
};
#endif
