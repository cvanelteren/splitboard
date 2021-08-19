#include "matrix.hpp"
// need to figure out some configuration
void print_byte(uint8_t value) {
  for (byte i = 0; i < 8; i++) {
    Serial.print((value & (1 << i)) ? 1 : 0);
    // Serial.print(!!(value & (1 << i))); //this should also work
  }
  Serial.println();
}

Matrix::Matrix(Config *config) {
  // setup pins
  // Serial.println("Setting up matrix");
  Serial.println("Setting up keyboard");
  this->source_pins =
      (config->scan_source == "row" ? config->row_pins : config->col_pins);
  this->sinc_pins =
      (config->scan_source == "row" ? config->col_pins : config->row_pins);
  this->debounce = config->debounce;

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
          keyswitch_t{millis(), false, 0, source, sinc, col, row};
    }
  }
}
void Matrix::setup_pins() {
  // set output pins to low by default
  for (auto source : this->source_pins) {
    pinMode(source, OUTPUT);
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
    // Serial.println();
  }
}

void Matrix::add_key(keyswitch_t &key) {
  /**
   * @brief      Checks if key is already in active keys.
   * If key is not foud, adds the key to @active_scan_keys.
   */
  bool add = true;
  for (auto &elem : this->active_keys) {
    if (elem.source == key.source && elem.sinc == key.sinc) {
      if (elem.pressed_down == key.pressed_down) {
        add = false;
      }
      break;
    }
  }
  if (add) {
    // this->active_scan_keys.push_back(key);
    this->active_keys.push_back(key);
  }
}

void Matrix::determine_activity(keyswitch_t *key) {
  /**
   * @brief      Debounce keys
   *
   * @details  Debouces the  keys,  i.e.  determine a  key's
   * activitiy through multiple time samples. This functions
   * pushes a debounced key into active_scan_keys.
   *
   * @param      ke, current matrix vertex being checked.
   *
   */

  // check pin state
  // @future me: the digital read needs to be reversed to the current being
  // opposite. See @scan method
  auto active = !digitalRead(key->sinc);

  // fill filter
  key->buffer <<= 1;
  if (active) {
    key->buffer |= 0x01;
  }

  // check if rising edge
  if ((key->buffer == 0xff) && (!key->pressed_down)) {
    key->pressed_down = true;
    key->time = millis();
    this->add_key(*key);
  }
  // check falling edge
  else if ((key->buffer == 0x00) && (key->pressed_down)) {
    // key->a_buff = 0;
    key->pressed_down = false;
    this->add_key(*key);
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

  // this->print_ak();

  // // Serial.println("Listing keys");
  // // Serial.println();
  // for (auto key : this->active_keys) {
  //   Serial.print("\r");
  //   Serial.print(key.source);
  //   Serial.print(key.sinc);
  //   Serial.println();
  // }
}

void callback(){
    // callback stuff
    // Serial.println("In touch callback");
};

void Matrix::sleep() {
  /**
   * @brief      prepare pins for sleeping.
   * @detail Note: This does not initiate the sleep
   */
  for (auto pin : this->source_pins) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    Serial.printf("%d %d \n", pin, touchRead(pin));
  }

  uint8_t threshold = 20;
  // IMPORTANT: set sleep pins high(!)
  for (auto pin : this->sinc_pins) {
    // mask += pow(2, pin);
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    touchAttachInterrupt(pin, callback, threshold);
    Serial.printf("%d %d \n", pin, touchRead(pin));
  }
}

// debug function
void Matrix::print_ak() {
  for (auto &source : this->source_pins) {
    for (auto &sinc : this->sinc_pins) {
      Serial.printf("%d (%d %d) %d %d ", this->keys[source][sinc].pressed_down,
                    digitalRead(source), digitalRead(sinc), source, sinc);
    }
    Serial.println();
  }
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

std::vector<uint8_t> Matrix::get_source_pins() { return this->source_pins; }
std::vector<uint8_t> Matrix::get_sinc_pins() { return this->sinc_pins; }
