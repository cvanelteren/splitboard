#ifndef ROTARY_ENCODER_H_
#define ROTARY_ENCODER_H_

#include "config.hpp"

#include <AiEsp32RotaryEncoder.h>

class RotaryEncoder : public AiEsp32RotaryEncoder {
  Config *config;

public:
  RotaryEncoder(Config *config);
  void onButtonClick();
  void update();
};

#endif // ROTARY_ENCODER_H_
