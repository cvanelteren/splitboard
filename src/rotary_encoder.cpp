#include "rotary_encoder.hpp"
#include <bitset>
#include <cstdio>

// // RotaryEncoder::RotaryEncoder(Config *config)
// //     : AiEsp32RotaryEncoder(config->rot_a_pin, config->rot_b_pin,
// //                            config->rot_button_pin, config->rot_output_pin,
// //                            config->rot_encoder_steps) {
// //
// this->config = config;

// // setup handlers
// // this->setup([this] { this->readEncoder_ISR(); },
// // [this] { this->onButtonClick(); });

// // circle values reset set to false
// this->setBoundaries(0, 1000, false);
// this->setAcceleration(config->rot_accel);
// this->reset();

// //   pinMode(config->rot_a_pin, INPUT_PULLUP);
// //   pinMode(config->rot_b_pin, INPUT_PULLUP);
// //   pinMode(config->rot_output_pin, OUTPUT);
// //   // pinMode(config->rot_button_pin, OUTPUT);
// }

RotaryEncoder::RotaryEncoder(Config *config) {
  this->config = config;
  pinMode(config->rot_a_pin, INPUT);
  pinMode(config->rot_b_pin, INPUT);

  pinMode(config->rot_a_pin, INPUT_PULLUP);
  pinMode(config->rot_b_pin, INPUT_PULLUP);
  pinMode(config->rot_output_pin, INPUT);
  // pinMode(2, INPUT_PULLUP);

  this->a_state = digitalRead(this->config->rot_a_pin);
  this->b_state = digitalRead(this->config->rot_b_pin);
  this->counter = 0;
  this->last_active_time = millis();

  this->state = 0;
  this->counter = 0;
  // this->prevNextCode = 0;
  // this->store = 0;
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
  if ((val = this->read_rotary())) {
    this->counter += val;
    Serial.println("-------------");
    Serial.printf("%d \t", this->counter);
    Serial.printf("%d %d \n", digitalRead(this->config->rot_a_pin),
                  digitalRead(this->config->rot_b_pin));
    Serial.println("-------------");
  }
}
