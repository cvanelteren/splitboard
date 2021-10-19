#include "led_driver.hpp"
#include <utility>

LED::LED(Config *config) {
  Serial.printf("Setting %d leds", config->num_led);
  this->update_func_ptr = &LED::cycle;
  this->num_leds = config->num_led;
  this->led_pin = Config::led_pin;
  this->leds = new CRGB[num_leds];
  this->led_col_bins = config->led_col_bins;

  this->brightness = 128;
  this->sleep();
  FastLED.setBrightness(brightness);

  this->active_keys = nullptr;
}

void LED::update() {
  static uint8_t status;
  static size_t time;

  size_t delta = millis() - time;
  // printf("\rDelta %d", delta);

  if (delta > 2000) {
    status = (status + 1) % 3;
    time = millis();
  }

  switch (status) {
  case 0: {
    this->update_func_ptr = &LED::serial_cycle;
    break;
  }
  case 1: {
    this->update_func_ptr = &LED::cycle;
    break;
  }
  case 2: {
    // this->update_func_ptr = &LED::ble_status();
    break;
  }
  case 3: {
    this->update_func_ptr = &LED::follow_me;
  }
  default:
    break;
  }

  this->update_func_ptr = &LED::follow_me;
  (this->*(update_func_ptr))();
  FastLED.show();
}

LED::~LED() { delete this->leds; }

void LED::begin() {
  pinMode(led_pin, OUTPUT);
  Serial.printf("Setting %d leds\n", num_leds);
  FastLED.addLeds<SK6812, Config::led_pin, GRB>(leds, num_leds);
  FastLED.show();
}

void LED::set_color(uint8_t hue, uint8_t saturation, uint8_t value) {
  // for (auto idx = 0; idx < num_leds; idx++) {
  // leds[idx].setHSV(hue, saturation, value);
  // }
}

void LED::wakeup() {
  Serial.printf("Setting LED brightness: %d \n", brightness);
  FastLED.setBrightness(brightness);
}
void LED::sleep() {
  Serial.printf("LED power down\n");
  FastLED.setBrightness(0);
  for (auto idx = 0; idx < num_leds; idx++) {
    leds[idx].setHSV(0, 0, 0);
  }
  FastLED.show();
}

void LED::cycle() {
  // rainbow effect
  static uint8_t hue, saturation, value;
  value = 50;
  // Serial.printf("%d \t %d \t %d \n", hue, saturation, value);
  // FastLED.showColor();

  // saturation++;
  saturation = 255; // primary colors
  hue++;
  for (auto idx = 0; idx < num_leds; idx++) {
    // leds[idx] = CRGB(hue, saturation, value);
    leds[idx].setHSV(hue, saturation, value);
  }
}

void LED::ble_status(bool connected) {
  static size_t time, delta;
  static uint8_t value;
  static bool connected_;
  static uint8_t counter;

  if (connected_ ^ connected) {
    connected_ = connected;
    if (connected_ == 0)
      this->sleep();
    else {
      FastLED.setBrightness(128);
      counter = 0;
    }
  }

  delta = millis() - time;
  // printf("Delta \t %d\n", delta);
  if ((delta > 1000) && (counter < 3)) {
    time = millis();
    value += 255;
    counter++;
    for (auto idx = 0; idx < 5; idx++) {
      leds[idx].setHSV(128, 255, value);
    }
  } else if (counter == 3)
    counter++;
  this->sleep();
}

void LED::serial_cycle() {
  static uint8_t hue, saturation, value;
  static uint8_t active_led;
  static size_t last_active;

  saturation = 255;

  for (uint8_t idx = 0; idx < num_leds; idx++) {
    if (active_led == idx) {
      leds[idx].setHSV(hue, saturation, value);
    } else {
      leds[idx].setHSV(0, 0, 0);
    }
  }
  auto delta = millis() - last_active;
  if (delta > 100) {
    active_led++;
    last_active = millis();
    // Serial.printf("Setting %d LED on\n", active_led);
    // Serial.printf("Delta \t %d \n", delta);
  }
  if (!(active_led % 5)) {
    hue++;
    value = 50;
    // value += 10;
  }
  if (active_led == num_leds) {
    active_led %= num_leds;
  }
}

void LED::follow_me() {
  // Follows the keypresses and activates that key
  size_t idx;
  FastLED.setBrightness(brightness);
  static size_t decay;
  // decay
  if ((millis() - decay) > 120) {
    sleep();
    decay = millis();
  }

  for (auto &key : (*active_keys)) {
    idx = grid2int(key.col, key.row);
    printf("Activating %d\n", idx);
    leds[idx].setHSV(128, 128, 128);
  }
}

uint8_t LED::grid2int(uint8_t row, uint8_t col) {
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

  printf("Received row %d col %d \n", row, col);
  // reverse the linear path as the columns start on the top left
  col = led_col_bins.size() - col - 2;
  bool uneven = col % 2;
  uint8_t idx = this->led_col_bins[col];
  if (uneven) {
    idx += (led_col_bins[col + 1] - led_col_bins[col]) - row - 1;
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
  return std::pair<uint8_t, uint8_t>(row, col);
}
