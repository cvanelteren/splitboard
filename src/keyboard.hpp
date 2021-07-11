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
#include "types.h"

#include <BleKeyboard.h>
class Keyboard {

public:
  Matrix *matrix;
  // Layout *layout;
  layers_t layers;
  Display *display;
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

  bool is_server;
  double get_battery_level();

private:
};
#endif
