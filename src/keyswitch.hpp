#ifndef keyswitch_hpp
#define keyswitch_hpp
#include "types.hpp"
#include <cstddef>
#include <stdint.h>

typedef struct {
  uint8_t source;
  uint8_t sinc;
  uint8_t col;
  uint8_t row;

} keyswitch_cfg_t;

class KeyMessage : public AbstractMessage {
public:
  KeyMessage(bool, uint8_t row, uint8_t col, size_t time);
  bool pressed_down;
  uint8_t row, col;
  size_t time;
};

class KeySwitch : public AbstractInput {

public:
  KeySwitch(keyswitch_cfg_t config);
  AbstractMessage create_msg() const override;

private:
  size_t time;       // activation time
  bool pressed_down; // key up or key down
  uint8_t buffer;    // debounce filter
  keyswitch_cfg_t config;
  bool debounce();

  friend class Matrix;
};

#endif
