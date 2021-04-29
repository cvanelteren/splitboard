
#ifndef CONFIG_CPP
#define CONFIG_CPP
#include <cstddef>
#include <string>
#include <vector>

#include <U8g2lib.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>

class Config {
  // see constructor in cpp file
public:
  Config();
  std::vector<size_t> row_pins;
  std::vector<size_t> col_pins;
  size_t debounce;
  std::string name;
  size_t pin_sda, pin_scl, pin_reset;
  const u8g2_cb_t *display_rotation;
};
#endif
