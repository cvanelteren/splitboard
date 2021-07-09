#ifndef CONFIG_CPP
#define CONFIG_CPP
#include <cstddef>
#include <string>
#include <vector>

#include <U8g2lib.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>

#include <unordered_map>

#include "types.h"
// #include <nlohmann/json.hpp>
// for convenience
// using json = nlohmann::json;

// layer_t c({{s, b}});

class Config { // see constructor in cpp file
public:
  Config();

  // pin configuration
  std::vector<uint8_t> row_pins = {18, 23, 19, 22, 21};
  // std::vector<size_t> row_pins = {13, 12, 14, 27, 26};
  // std::vector<size_t> col_pins = {18, 23, 19, 22, 21};
  // std::vector<size_t> col_pins = {34, 39, 37, 36};
  std::vector<uint8_t> col_pins = {13, 12, 14, 27, 26};
  bool row2col = true; // diode direction

  layer_t test = {{18, {{34, "A"}}}};
  // layers_t layers{{1, test}};

  uint8_t led_pin = 25;
  uint8_t num_led = 1;
  std::vector<uint8_t> frame_buffer = std::vector<uint8_t>(num_led, 0);
  std::vector<uint8_t> draw_buffer = std::vector<uint8_t>(num_led, 0);

  size_t debounce = 30;

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

  // change this for your device
  // const char *server_address = "80:7d:3a:d4:2e:46";
  const char *client_address = "80:7D:3A:D4:2E:44";
  uint8_t client_add[6] = {0x80, 0x7D, 0x3A, 0xD4, 0x2E, 0x44};
  // uint8_t serv_add[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  const char *server_address = "80:7D:3A:D4:2C:9C";
  uint8_t serv_add[6] = {0x80, 0x7D, 0x3A, 0xD4, 0x2C, 0x9C};
  // uint8_t client_add[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
};
#endif
