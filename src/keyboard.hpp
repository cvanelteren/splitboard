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
#include "display.hpp"
#include "layout.hpp"

class Keyboard {

public:
  Matrix *matrix;
  Layout *layout;
  Display *display;

  Keyboard(Config *config);
  void begin();
  BleKeyboard ble;
};
#endif
