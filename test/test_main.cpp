#ifdef UNIT_TEST
#include "utils.hpp"
#include <Arduino.h>
#include <unity.h>

#include "config.hpp"
#include "matrix.hpp"

Config *config = new Config();
Matrix *matrix = new Matrix(config);

// Matrix tests
void test_source_pin_mode() {
  for (auto pin : matrix->get_source_pins()) {
    TEST_ASSERT_EQUAL(get_pin_mode(pin), OUTPUT);
  }
};

void test_sinc_pin_mode() {
  for (auto pin : matrix->get_sinc_pins()) {
    TEST_ASSERT_EQUAL(get_pin_mode(pin), INPUT_PULLUP);
  }
};

void test_zero_state_keys() {
  for (auto &source : matrix->get_source_pins()) {
    for (auto &sinc : matrix->get_sinc_pins()) {
      TEST_ASSERT_EQUAL(matrix->keys[source][sinc].pressed_down, 0);
    }
  }
}

void test_ghosting() {
  /**
   * @brief      Test artificial ghosting signal
   * @details    Ghosting occurs if diodes are not placed correctly.
   * Current flows back into a channel that is not acivated.
   */

  for (auto idx = 0; idx < 2; idx++) {
    digitalWrite(matrix->source_pins[idx], HIGH);
    digitalWrite(matrix->sinc_pins[idx], HIGH);
  }
  digitalWrite(matrix->sinc_pins[1], LOW);
  TEST_ASSERT_EQUAL(digitalRead(matrix->sinc_pins[1]), HIGH);
}

// end Matrix tests
void setup() {
  UNITY_BEGIN();

  // test the matrix
  RUN_TEST(test_source_pin_mode);
  RUN_TEST(test_sinc_pin_mode);
  RUN_TEST(test_zero_state_keys);
  RUN_TEST(test_ghosting);
}
void loop() { UNITY_END(); }
#endif
