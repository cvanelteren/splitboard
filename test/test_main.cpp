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

uint8_t number_of_pressed_keys() {
  static uint8_t key_pressed_down;
  matrix->update();
  for (auto elem : matrix->active_keys) {
    if (elem.pressed_down) {
      key_pressed_down += 1;
    } else {
      key_pressed_down -= 1;
    }
  }
  // delay(100);
  return key_pressed_down;
}

void test_ghosting() {
  /**
   * @brief      Test artificial ghosting signal
   * @details    Ghosting occurs if diodes are not placed correctly.
   * Current flows back into a channel that is not acivated.
   */

  uint8_t threshold, num_keys;
  threshold = 3;
  // wait for button press
  Serial.println("Please hold three keys");
  Serial.println("Waiting..");

  size_t last_state = 0;

  while ((num_keys = number_of_pressed_keys()) < threshold) {
    // if ((millis() - last_state) > 1000) {
    //   Serial.printf("%d num_keys\n", num_keys);
    //   last_state = millis();
    // }
  }

  for (auto source : matrix->source_pins) {
    pinMode(source, OUTPUT);
    digitalWrite(source, LOW);
  }

  for (auto sinc : matrix->sinc_pins) {
    pinMode(sinc, INPUT_PULLUP);
  }
  TEST_ASSERT_EQUAL(number_of_pressed_keys(), threshold);
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
