#ifndef KEYBOARD_CPP
#define KEYBOARD_CPP
#include <Arduino.h>

// bluetooth keyboard holding
#include <BleKeyboard.h>

#include <cstddef>
#include <unordered_map>
#include <utility>

// holds configuration for the entire keyboard
#include "config.hpp"
// internal to readout the pins
#include "matrix.hpp"
// holds layout keys
#include "bluetooth.hpp"
#include "display.hpp"
#include "layout.hpp"
#include "mesh.hpp"
#include "types.hpp"

#include <BleKeyboard.h>
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
  Matrix *matrix;
  // Layout *layout;
  layers_t layers;
  Display *display;
  Config *config;
  // std::vector<uint8_t> fb;

  // Bluetooth *bluetooth;
  BleKeyboard *bluetooth;
  Mesh *mesh;
  // BleKeyboard ble;

  Keyboard(Config *config);
  void begin();
  void update();
  // communicate with bluetooth
  void send_keys();

  uint8_t read_key(keyswitch_t &keyswitch);

  bool is_server;
  double get_battery_level();

private:
  layer_t *active_layer;
};
#endif
