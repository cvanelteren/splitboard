#include <Arduino.h>
#include <unity.h>

#include "matrix.cpp"

int get_pin_mode(uint8_t pin) {
  if (pin >= NUM_DIGITAL_PINS)
    return (-1);

  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint8_t *reg = portModeRegister(port);
  if (*reg & bit)
    return (OUTPUT);

  volatile uint8_t *out = portOutputRegister(port);
  return ((*out & bit) ? INPUT_PULLUP : INPUT);
}

void test_source_pin_mode() {
  for (auto pin : matrix->source_pins) {
    TEST_ASSERT_EQUAL(get_pin_mode(pin), INPUT_PULLUP);
  }
};

void test_sinc_pin_mode() {
  for (auto pin : matrix->source_pins) {
    TEST_ASSERT_EQUAL(get_pin_mode(pin), OUTPUT);
  }
};
