#include "led_driver.hpp"
#include <utility>

LED::LED(Config *config) {
  this->num_leds = config->num_led;
  this->led_pin = Config::led_pin;
  this->leds = new CRGB(config->num_led);
  this->led_col_bins = config->led_col_bins;

  // this->saturation = 0;
  // this->hue = 0;
  // this->value = 0;
  this->brightness = 128;
  FastLED.setBrightness(brightness);
}

LED::~LED() { delete this->leds; }

void LED::begin() {
  pinMode(led_pin, OUTPUT);
  FastLED.addLeds<SK6812, Config::led_pin, GRB>(leds, num_leds);
  FastLED.show();
}

void LED::set_color(uint8_t hue, uint8_t saturation, uint8_t value) {
  for (auto idx = 0; idx < num_leds; idx++) {
    leds[idx].setHSV(hue, saturation, value);
  }
}

void LED::wakeup() {
  Serial.printf("Setting LED brightness: %d \n", brightness);
  FastLED.setBrightness(brightness);
}
void LED::sleep() {
  FastLED.setBrightness(0);
  // for (auto idx = 0; idx < num_leds; idx++) {
  // leds[idx].setBrightness(0);
  // }
  FastLED.show();
}

void LED::cycle() {
  // rainbow effect
  static uint8_t hue, saturation, value, hue2;
  value = 50;
  // Serial.printf("%d \t %d \t %d \n", hue, saturation, value);
  // FastLED.showColor();

  // saturation++;
  saturation = 255; // primary colors
  hue++;
  hue2--;
  for (auto idx = 0; idx < num_leds; idx++) {
    if (idx == 1) {
      leds[idx].setHSV(hue2, saturation, value);
    } else {
      leds[idx].setHSV(hue, saturation, value);
    }
  }
  FastLED.show();
}

uint8_t LED::grid2int(std::pair<uint8_t, uint8_t> grid) {
  /**
   * @details  The  leds  are   wired  like  a  snake.  Each
   * keyswitch has  an LED (except the  rotary encoder). The
   * linear indices need  to be converted to  a regular grid
   * coordinate for easier interfacing for complex animation
   * patterns.
   *
   * Since the grid is not-regular, bins are used to correct
   * for the number of columns the snake is in. In addition,
   * the couting for the particular row needs to be adjusted
   * depending on the parity of the columns.
   *
   * The snake  adjusts down first, so  uneven columns start
   * from the bottom  (number of rows for  that column), and
   * even columns start from the top of the grid.
   *
   *
   * @param      grid point
   * @return     linear index
   */

  auto col = grid.second;
  auto row = grid.first;
  bool uneven = col % 2;

  auto idx = this->led_col_bins[col];
  if (uneven) {
    idx += this->led_col_bins.size() - row;
  } else {
    idx += row;
  }
  return idx;
}

std::pair<uint8_t, uint8_t> LED::int2grid(uint8_t idx) {
  // reversed of above
  uint8_t col, row;
  for (auto start = 0, end = 1; start < led_col_bins.size(); start++, end++) {
    if (led_col_bins[start] <= idx < led_col_bins[end]) {
      col = start;
      if (start % 2) {
        row = led_col_bins.size() - (idx - led_col_bins[start]);
      } else {
        row = idx - led_col_bins[start];
      }
    }
  }
  return std::make_pair<uint8_t, uint8_t>(row, col);
}
