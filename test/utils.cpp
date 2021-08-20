#include "utils.hpp"
#include <Arduino.h>

uint8_t get_pin_mode(uint8_t pin) {
  if (pin >= NUM_DIGITAL_PINS)
    return (-1);

  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint32_t *reg = portModeRegister(port);
  if (*reg & bit) {
    return (OUTPUT);
  }

  volatile uint32_t *out = portOutputRegister(port);
  return ((*out & bit) ? INPUT_PULLUP : INPUT);
}
