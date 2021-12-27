#include "keyboard_events.hpp"
#include "keyboard.hpp"

extern Keyboard keyboard;
bool set_led_serial_cycle() {
  return keyboard.led->set_mode(LED_SERIAL_CYCLE);
  // TODO: add mesh communication for cycle
}

bool set_led_cycle() {
  return keyboard.led->set_mode(LED_CYCLE);
  // TODO: add mesh communication for cycle
}

// add mesh part
bool increase_led_brightness() {
  return keyboard.led->increase_brightness();
  // TODO: add mesh communication
}
bool decrease_led_brightness() {
  return keyboard.led->decrease_brightness();
  // TODO: add mesh communication
}

bool display_battery_level() {
  double bat_level = keyboard.get_battery_level();
  printf("Battery level= %f\n");
  // TODO: instruct display to show the battery mode
  return false;
}
