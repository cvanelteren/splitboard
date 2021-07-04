#include "display.hpp"
Display::Display(Config *config)
    : U8G2_SSD1306_128X64_NONAME_F_HW_I2C(config->display_rotation,
                                          config->pin_reset, config->pin_scl,
                                          config->pin_sda) {

  // setup logger
  this->log_buffer = std::vector<uint8_t>(0);
}
