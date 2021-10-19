#ifndef LED_HPP
#define LED_HPP

#include "config.hpp"
#include <FastLED.h>
#include <cstdint>
#include <stdint.h>
#include <vector>
class LED {
  uint8_t num_leds;
  uint8_t led_pin;
  uint8_t brightness;
  CRGB *leds;

private:
  std::vector<uint8_t> led_col_bins;
  std::vector<size_t> last_activity;
  typedef void (LED::*update_method)();
  update_method update_func_ptr;

public:
  void begin();
  LED(Config *Config);
  ~LED();
  void cycle();
  void serial_cycle();
  void follow_me();
  void set_color(uint8_t hue, uint8_t saturation, uint8_t value);
  void ble_status(bool connected);
  void turn_off_all();
  void battery_level();

  // TODO: make some other interface for this
  std::vector<keyswitch_t> *active_keys;

  void update();

  // drive specific locations
  std::pair<uint8_t, uint8_t> int2grid(uint8_t idx);
  uint8_t grid2int(uint8_t row, uint8_t col);
  void sleep();
  void wakeup();
};
#endif
