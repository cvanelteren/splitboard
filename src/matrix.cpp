#include "matrix.hpp"
// need to figure out some configuration
Matrix::Matrix(Config *config) {
  // setup pins
  // Serial.println("Setting up matrix");
  this->row2col = config->row2col;
  this->source_pins = (this->row2col ? config->row_pins : config->col_pins);
  this->sinc_pins = (this->row2col ? config->col_pins : config->row_pins);
  this->debounce = config->debounce;

  // empty active_keys on init
  // this->active_keys.clear();
  Serial.println("Setting pins");

  this->setup_pins();

  this->setup_keys();
}

void Matrix::setup_keys() {

  for (auto source : this->source_pins) {
    for (auto sinc : this->sinc_pins) {
      // init switch with current time, non-active and pin info
      this->keys[source][sinc] = keyswitch_t{millis(), false, source, sinc};
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
void Matrix::scan() {
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
  }
}

void Matrix::determine_activity(keyswitch_t *key) {
  /* Determine the activity of the keys and puts them in the unordered map */
  // check for debounce active keys
  // check for debounce

  // std::cout << key->first << key->second;
  // std::cout << std::endl;
  // std::cout << digitalRead(key->first) << digitalRead(key->second);
  // std::cout << std::endl;

  // check pin state
  auto active = digitalRead(key->sinc) ? false : true;

  // this line doesnot check that it was already active before
  // if was active just remain active
  if (active & key->active) {
    if ((millis() - key->time) >= this->debounce) {
      this->active_keys.push_back(*key);
    }
  } else if (active & !key->active) {
    key->active = true;
    key->time = millis();
  } else {
    key->active = false;
  }
}

void Matrix::update() {
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
      Serial.println(this->keys[source][sinc].active);
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
