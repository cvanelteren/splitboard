#include "matrix.hpp"
// need to figure out some configuration
Matrix::Matrix(Config *config) {
  // setup pins
  // Serial.println("Setting up matrix");
  Serial.println("Setting up keyboard");
  this->row2col = config->row2col;
  this->source_pins = (this->row2col ? config->row_pins : config->col_pins);
  this->sinc_pins = (this->row2col ? config->col_pins : config->row_pins);
  this->debounce = config->debounce;

  // empty active_keys on init
  // this->active_keys.clear();
  //

  // activate pins
  this->setup_pins();
  // initialize keys
  this->setup_keys();
}

void Matrix::setup_keys() {
  // Sets all keys to non-active
  uint8_t source, sinc;
  for (uint8_t row = 0; row < this->source_pins.size(); row++) {
    source = this->source_pins[row];
    for (uint8_t col = 0; col < this->sinc_pins.size(); col++) {
      // init switch with current time, non-active and pin info
      sinc = this->sinc_pins[col];
      this->keys[source][sinc] =
          keyswitch_t{millis(), false, source, sinc, col, row};
    }
  }
}
void Matrix::setup_pins() {
  // set output pins to low by default
  for (auto source : this->source_pins) {
    pinMode(source, OUTPUT);
    // digitalWrite(source, LOW);
  }
  // initialize input pins
  for (auto sinc : this->sinc_pins) {
    pinMode(sinc, INPUT_PULLUP);
  }
}

// todo implement coroutine or async routine for measuring minimum debounce
// i need a way to track the pin state over a time
// the ide
//
void Matrix::scan() {
  /**
   * @brief      Scans keyboard pins sequentially
   *
   * @details Scanning  consists of determining  the current
   * active key by debouncing. In addition, repeated presses
   * are filtered out;  keeping key pressed down  has a cool
   * down period after which at  regular interval the key is
   * sent.
   *
   * Active keys are tracked through three arrays
   * - active_keys: actual double debounced keys
   * - active_scan_keys: debounced keys
   * - past_scan_keys: debounce buffer for repeated key presses
   */
  /* Determines the activity of the pins */
  keyswitch_t *key;
  this->active_keys.clear();
  this->active_scan_keys.clear();
  // Serial.println(this->active_keys.size());
  // check switch by setting source pin to high
  for (auto source : this->source_pins) {
    digitalWrite(source, LOW);
    for (auto sinc : this->sinc_pins) {
      // read switch
      pinMode(sinc, INPUT_PULLUP);
      key = &this->keys[source][sinc];
      this->determine_activity(key);
      pinMode(sinc, INPUT);
    }
    // reset source pin
    digitalWrite(source, HIGH);
  }

  // push into active
  this->determine_change();
}

void Matrix::determine_change() {
  /**
   * @brief Debounces the active_scan_keys to prevent double
   * presses.
   *
   * @details Keys  need debouncing  as the activation  of a
   * key is  not a perfect  square wave. The  debounced keys
   * are activated when  a change of signal  occurs. That is
   * at  the onset  of a  signal. This  results in  repeated
   * button  presses. This  function filters  those repeated
   * button presses out, and resends  a key after the key is
   * pressed down for some threshold.
   *
   * This function pushes a key into active_keys.
   */
  bool update_key = false;

  // keep track of overlap
  size_t delta;
  keyswitch_t key, old_key;
  // TODO: move back to range based loops?
  for (size_t idx = 0; idx < this->active_scan_keys.size(); idx++) {
    update_key = true;
    key = this->active_scan_keys[idx];

    for (size_t jdx = 0; jdx < this->past_scan_keys.size(); jdx++) {
      old_key = this->past_scan_keys[jdx];
      if ((key.source == old_key.source) && (key.sinc == old_key.sinc)) {
        // compare debounce on keys

        delta = millis() - key.time;
        // FIXME: left number is when the debounce will be repeated
        // bit of a magic number, move into config?
        if ((delta >= 500) && (!(delta % (this->debounce * 10)))) {
          update_key = true;
        } else {
          // don't
          update_key = false;
          // keep the old activation time
          this->active_scan_keys[idx] = this->past_scan_keys[jdx];
        }
        // break out  the loop
        break;
      }
    }
    if (update_key) {
      // push to bluetooth or other half
      Serial.println("Pushing a key");
      this->active_keys.push_back(this->active_scan_keys[idx]);
    }
  }
  this->active_scan_keys.swap(this->past_scan_keys);
}

void Matrix::determine_activity(keyswitch_t *key) {
  /**
   * @brief      Debounce keys
   *
   * @details  Debouces the  keys,  i.e.  determine a  key's
   * activitiy through multiple time samples. This functions
   * pushes a debounced key into active_scan_keys
   *
   * @param      key, current matrix vertex being checked.
   *
   */

  // check pin state
  auto active = digitalRead(key->sinc) ? false : true;

  // this line doesnot check that it was already active before
  // if was active just remain active
  if (active && key->active) {
    if ((millis() - key->time) >= this->debounce) {
      this->active_scan_keys.push_back(*key);
    }
  }
  // key switch turned on with debounce
  else if (active && !key->active) {
    key->active = true;
    key->time = millis();
  }

  // key switch turned off
  else if (!active && key->active) {
    if ((millis() - key->time) >= this->debounce) {
      key->active = false;
      // key->time = millis();
      this->active_keys.push_back(*key);
    }
  }

  // turn key off
  else {
    key->active = false;
  }
}

void Matrix::update() {
  /**
   * @brief      Matrix update loop
   *
   * @details Put all relevant  functions that need repeated
   * calls in here.
   *
   */
  this->scan();
  // this->print_ak();

  // Serial.println("Listing keys");
  // Serial.println();
  for (auto key : this->active_keys) {
    Serial.print("\r");
    Serial.print(key.source);
    Serial.print(key.sinc);
    Serial.println();
  }
}

// debug function
void Matrix::print_ak() {
  for (auto source : this->source_pins) {
    for (auto sinc : this->sinc_pins) {
      Serial.print(this->keys[source][sinc].active);
      Serial.print(" ");
      Serial.print(digitalRead(source));
      Serial.print(" ");
      Serial.print(digitalRead(sinc));
      Serial.print(" ");

      Serial.print("(");
      Serial.print(source);
      Serial.print(" ");
      Serial.print(sinc);
      Serial.print(")");

      // Serial.print(" ");
      // Serial.print(digitalRead(source) & digitalRead(sinc));
      // Serial.print(" ");
      // Serial.print(digitalRead(sinc));
      Serial.print(" ");
    }
    Serial.println();
  }

  // Serial.print("\r");

  // Serial.println("Listing active keys");
  // for (auto el : this->active_keys) {
  //   Serial.print(el.source);
  //   Serial.print(" ");
  //   Serial.print(el.sinc);
  //   Serial.println();
  // }
}

// debug function
void Matrix::show_switch(keyswitch_t *key) {
  Serial.print(key->source);
  Serial.print(" ");
  Serial.print(key->sinc);
  Serial.print(" ");
  Serial.print(digitalRead(key->source));
  Serial.print(digitalRead(key->sinc));
  Serial.println();
}

uint8_t Matrix::get_cols() { return this->source_pins.size(); }
uint8_t Matrix::get_rows() { return this->sinc_pins.size(); }
