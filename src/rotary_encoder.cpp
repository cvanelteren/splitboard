#include "rotary_encoder.hpp"
#include <bitset>
#include <cstdio>

RotaryEncoder::RotaryEncoder(Config *config) {
  this->config = config;

  pinMode(config->rot_a_pin, INPUT);
  pinMode(config->rot_b_pin, INPUT);

  pinMode(config->rot_a_pin, INPUT_PULLUP);
  pinMode(config->rot_b_pin, INPUT_PULLUP);
  pinMode(config->rot_output_pin, INPUT);

  this->a_state = digitalRead(this->config->rot_a_pin);
  this->b_state = digitalRead(this->config->rot_b_pin);
  this->counter = 0;
  this->last_active_time = millis();

  this->state = 0;   // holds the current active sequence
  this->counter = 0; // holds the value of the encoder
  this->encoder_state = {.time = 0,
                         .pressed_down = false,
                         .buffer = 0,
                         .source = config->rot_a_pin,
                         .sinc = config->rot_b_pin,
                         .col = 255,
                         .row = 255};
}

void RotaryEncoder::begin() {}

void RotaryEncoder::onButtonClick() { Serial.println("button pressed"); }

int8_t RotaryEncoder::read_rotary() {
  bool a = digitalRead(this->config->rot_a_pin);
  bool b = digitalRead(this->config->rot_b_pin);

  // fill buffer with zeros from the right
  this->state <<= 2;
  if (a)
    this->state |= 0x02;
  if (b)
    this->state |= 0x01;
  // unset all bits that are not set --> the number is uint8,but
  // only 4 bits are used to encode!
  this->state &= 0x0f;

  // check for last 2 states in
  // the quadrature signal
  if (this->enc_table[this->state]) {
    // Serial.println(prevNextCode, DEC);
    this->filter <<= 4;
    // set first 4 bits
    this->filter |= this->state;
    // remove last 8 bits
    this->filter &= 0xff;
    // counter-clockwise quadrature
    if (this->filter == 0x2b)
      return -1;
    // clockwise quadrature
    if (this->filter == 0x17)
      return 1;
  }
  return 0;
}

void RotaryEncoder::update() {
  int8_t val;
  this->active_keys.clear();
  // add vol
  if ((val = this->read_rotary())) {
    if (val == -1) {
      this->encoder_state.buffer = 2;
    } else {
      this->encoder_state.buffer = 1;
    }
    this->active_keys.push_back(this->encoder_state);
    this->counter += val;
    Serial.println("-------------");
    Serial.printf("active_keys %d\n", this->active_keys.size());
    Serial.printf("%d \t", this->counter);
    Serial.printf("%d %d \n", digitalRead(this->config->rot_a_pin),
                  digitalRead(this->config->rot_b_pin));
    Serial.println("-------------");
  }
}
