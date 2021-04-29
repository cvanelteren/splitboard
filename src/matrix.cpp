#include "matrix.hpp"
// need to figure out some configuration
Matrix::Matrix(Config *config) {
  // setup pins
  this->row_pins = config->row_pins;
  this->col_pins = config->col_pins;
  this->debounce = config->debounce;
  // empty active_keys on init
  this->active_keys.clear();
}

void Matrix::scan() {
  /* Determines the activity of the pins */
  switch_t key;
  for (auto pin_row : this->row_pins) {
    key.first = pin_row;
    for (auto pin_col : this->col_pins) {
      key.second = pin_col;
      this->determine_activity(&key);
    }
  }
}

void Matrix::determine_activity(switch_t *key) {
  /* Determine the activity of the keys and puts them in the unordered map */
  // check for debounce active keys
  try {
    if (millis() - this->active_keys.at(*key) > this->debounce) {
      this->active_keys[*key] = millis();
    }
  } catch (std::out_of_range) {
    // add to active if both are active
    if (digitalRead(key->first) && digitalRead(key->second))
      this->active_keys[*key] = millis();
    // if both not active remove it
    else
      this->active_keys.erase(*key);
  }
}
