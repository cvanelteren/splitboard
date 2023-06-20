#include "display.hpp"
#include <Arduino.h>

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

u8g2_uint_t offset;

template <typename T>
bool inBounds(const T &value, const T &low, const T &high) {
  return !(value < low) && (value < high);
}

void Display::draw_battery(double voltage) {
  this->setFont(u8g2_font_battery19_tn);

  // there are 6 images for battery where the last is charging icon
  double lower = 0;
  double upper = 20;
  double step = 20;
  double symbol_start = 48;
  while (lower < 100) {
    if (inBounds(voltage, lower, upper)) {
      break;
    }
    lower = upper;
    upper += step;
    symbol_start++;
  }
  // battery icons are 10 x 19
  int x = this->getDisplayWidth() - 10;
  int y = 20;
  // draw in the top right
  this->drawGlyph(x, y, symbol_start);
}

void Display::draw_text(String text, int y = 50) {
  // scrolling text temp
  static int offset;
  int width = this->getUTF8Width(text.c_str());
  int x = 0;
  do {
    x += width;
    this->setFont(u8g2_font_inb30_mr);
    this->drawUTF8(x, y, text.c_str());
  } while (x < this->getDisplayWidth());

  offset -= 2; // scroll by one pixel
  if ((u8g2_uint_t)offset < (u8g2_uint_t)-width)
    offset = 0; // start over again
}

void Display::update(double battery_level) {

  u8g2_uint_t x;
  const char *text = "testing";
  this->setFont(u8g2_font_inb30_mr);

  this->firstPage();
  do {
    this->draw_battery(battery_level);
  } while (this->nextPage());
}
