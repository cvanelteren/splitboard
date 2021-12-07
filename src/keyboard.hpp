#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <BleKeyboard.h>
// bluetooth keyboard holding
#include <cstddef>
#include <unordered_map>
#include <utility>

// holds configuration for the entire keyboard
#include "config.hpp"
// internal to readout the pins
#include "matrix.hpp"
// holds layout keys
#include "display.hpp"
#include "keymap.hpp"
#include "layout.hpp"
#include "led_driver.hpp"
#include "mesh.hpp"
#include "rotary_encoder.hpp"
#include "types.hpp"

// #include "bluetooth.hpp"
class Keyboard {
  /**
   * @brief      Main keyboard class
   *
   * @details Binds together  all the necessary ingredients.
   * This includes holding the  mesh for esp-now, bluetooth,
   * and matrix  related function. This should  be the class
   * that a user interfaces with.
   */
public:
  Config *config;

  Matrix *matrix;
  Mesh *mesh;
  Display *display;
  LED *led;
  BleKeyboard bluetooth;

  RotaryEncoder *rotary_encoder;

  Keyboard(Config *config);
  void begin();
  void update();
  // communicate with bluetooth
  void process_keyswitches();
  void process_keyswitch(keyswitch_t &keyswitch, bool add_special);

  // special keycode functions
  void process_special_keycodes();
  void keep_active(keyswitch_t &keyswitch);

  bool is_server;
  double get_battery_level();

  layers_t layers;
  void sleep();
  void wakeup();

private:
  layer_t *active_layer;
  uint8_t active_layer_num;
  size_t last_activity;
  std::unordered_map<std::string,
                     std::unordered_map<std::string, const MediaKeyReport *>>
      encoder_codes;
  std::vector<keyswitch_t> special_keycodes;

  size_t get_last_activity();
  void set_active_layer(uint8_t layer);
};
#endif
