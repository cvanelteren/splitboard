#include "rotary_encoder.hpp"
RotaryEncoder::RotaryEncoder(Config *config)
    : AiEsp32RotaryEncoder(config->rot_a_pin, config->rot_b_pin,
                           config->rot_button_pin, config->rot_output_pin,
                           config->rot_encoder_steps) {
  this->config = config;

  // setup handlers
  // this->setup([this] { this->readEncoder_ISR(); },
  // [this] { this->onButtonClick(); });

  // circle values reset set to false
  this->setBoundaries(0, 1000, false);
  this->setAcceleration(config->rot_accel);
  this->reset();

  // pinMode(config->rot_a_pin, OUTPUT);
  // pinMode(config->rot_b_pin, INPUT);
  pinMode(config->rot_output_pin, OUTPUT);
  pinMode(config->rot_button_pin, OUTPUT);
}

void RotaryEncoder::onButtonClick() { Serial.println("button pressed"); }

void RotaryEncoder::update() {
  // read encoder
  if (!this->encoderChanged()) {
    return;
  }

  Serial.printf("Value :%d\n", this->readEncoder());
}
