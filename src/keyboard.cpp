#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP
#include "keyboard.hpp"

Keyboard::Keyboard(Config *config) {
  this->matrix = new Matrix(config);
  this->layout = new Layout(config);
  this->display = new Display(config);
  // this->ble = BleKeyboard(config->name);
}

// start the keyboard
void Keyboard::begin() {
  this->ble.begin();
  this->display->begin();
}
#endif
