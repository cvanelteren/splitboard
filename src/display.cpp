#include "display.hpp"

Display::Display(Config *config)
    : U8G2_SSD1306_128X64_NONAME_F_HW_I2C(config->display_rotation,
                                          config->pin_reset, config->pin_scl,
                                          config->pin_sda) {

  // setup logger
  Serial.println("Setting up display");
  this->log_buffer = std::vector<uint8_t>(0);
  this->log.setRedrawMode(0);
  // this->setVComDeselect(0);
  // this->setPreChargePeriod(15, 1);
  // this->setContrast(0);
  // this->setBrightness(0);
}

void Display::sleep() { this->setPowerSave(1); }
void Display::wakeup() { this->setPowerSave(0); }

void Display::setVComDeselect(uint8_t v) { this->sendF("ca", 0x0db, v << 4); }

void Display::setPreChargePeriod(uint8_t p1, uint8_t p2) {
  this->sendF("ca", 0x0d9, (p2 << 4) | p1);
}
