#ifndef CONFIG_CPP
#define CONFIG_CPP
#include "types.hpp"
#include <U8g2lib.h>
#include <cstddef>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <string>
#include <unordered_map>
#include <vector>

// #include <nlohmann/json.hpp>
// for convenience
// using json = nlohmann::json;

// layer_t c({{s, b}});
// layout keycode (16 bits):
//- 8 bits of keycodes
//- 4 bits of layer info
//- 4 bits of special keys
#define LAYER_TAP 0b0001
#define MOD_TAP 0b0010
#define ONESHOT 0b0011

// TODO: move these to a "normal" key range
// TODO: check if this define does not cause an issue
#define KC_SLEEP 0x88
#define KC_TRNS 0x89

#define LAYER_TAP_DELAY_MS 150
#define ONESHOT_TIMEOUT 100

// read layer, shift it in the 4 bits of layer
#define LT(layer, kc) (kc | (LAYER_TAP << 12) | ((layer & 0xF) << 8))
// mods start from > 128. This is outside the common ascii range
// We only look at the first 6 bits and shift it into the proper range
#define MT(mod, kc) (kc | (MOD_TAP << 12) | (((mod)&0x1F) << 8))

// --- Configuration ---
// TODO: tmp defs to use the features
// #define USE_SLEEP 1
#define USE_ENCODER
#define USE_LED
#define USE_OLED

// battery voltage divider
#define R1 100.0    // in kOhm
#define R2 100.0    // in kOhm
#define ADC_REF 1.1 // V
#define VMAX 4200.0 // mV
#define VMIN 3300.0 // mV
#define VTOADC(in) ((ADC_REF * (in) / 4096.0))
#define VOUT(vin) (((vin)*R2) / (R1 + R2)) // conversion to dividor voltage
#define BAT_MAX_ADC VTOADC(VOUT(VMAX))
#define BAT_MIN_ADC VTOADC(VOUT(VMIN))

class Config { // see constructor in cpp file
public:
  Config();

  // pin configuration
  std::vector<uint8_t> col_pins = {18, 23, 19, 22, 21, 26};
  std::vector<uint8_t> row_pins = {0, 13, 12, 14, 27};
  std::string scan_source = "col"; // diode direction

  // battery pin
  // IMPORTANT: the pin needs to be connected to a voltage divider
  // Connecting the battery directly to a pin, will damage the GPIO.
  // Any ADC pin can be used.
  static const uint8_t batt_pin = 37;
  // deep sleep settings
  const size_t deep_sleep_timeout = 10;
  // 300000; // 5 minutes
  // const size_t deep_sleep_timeout = 3000; // 5 minutes

  // led settings
  static const uint8_t led_pin = 25;
  uint8_t num_led = 27;
  std::vector<uint8_t> led_col_bins = {0,  5,  10, 15,
                                       19, 23, 27}; // count from the LED11 pin
                                                    // add the final number of
                                                    // keys for good binning

  std::vector<uint8_t> frame_buffer = std::vector<uint8_t>(num_led, 0);
  std::vector<uint8_t> draw_buffer = std::vector<uint8_t>(num_led, 0);

  // info advertising bluetooth
  std::string device_manufacturer = "Casper van Elteren";
  std::string device_name = "SplitBoard";

  // display settings
  std::string name = "SplitBoard";
  uint8_t pin_sda = GPIO_NUM_4;
  uint8_t pin_scl = GPIO_NUM_15;
  uint8_t pin_reset = GPIO_NUM_16;
  // rotation 0 is horizontal
  const u8g2_cb_t *display_rotation = U8G2_R3;
  uint baud_rate = 115200;

  uint8_t rot_a_pin = 17;
  uint8_t rot_b_pin = 5;
  uint8_t rot_button_pin = 37;
  uint8_t rot_output_pin = 0; // set to -1 to use vcc

  uint8_t rot_encoder_steps = 4;
  uint8_t rot_accel = 250;
  // change this for your device
  // const char *server_address = "80:7d:3a:d4:2e:46";
  // uint8_t serv_add[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  // uint8_t client_add[6] = {0x80, 0x7D, 0x3A, 0xD4, 0x2E, 0x44};
  // uint8_t serv_add[6] = {0x80, 0x7D, 0x3A, 0xD4, 0x2E, 0x44};
  uint8_t serv_add[6] = {0x80, 0x7D, 0x3A, 0xD4, 0x2C, 0x9C};
  // uint8_t client_add[6] = {0x80, 0x7D, 0x3A, 0xD4, 0x2C, 0x9C};
  // uint8_t client_add[6] = {0x80, 0x7D, 0x3A, 0xD4, 0x2C, 0x9C};
  // uint8_t serv_add[6] = {0x80, 0x7D, 0x3A, 0xD4, 0x2C, 0x9C};
  // uint8_t serv_add[6] = {0x7C, 0x9E, 0xBD, 0xFB, 0xDA, 0xD4};
  uint8_t client_add[6] = {0x7C, 0x9E, 0xBD, 0xFB, 0xD9, 0x78};
  // uint8_t client_add[6] = {0x7C, 0x9E, 0xBD, 0xFB, 0xDA, 0xD4};
  // uint8_t serv_add[6] = {0x7C, 0x9E, 0xBD, 0xFB, 0xDA, 0xD4};
  // uint8_t serv_add[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  // uint8_t serv_add[6] = {0x7C, 0x9E, 0xBD, 0xFB, 0xDA, 0xD4};
};
#endif
