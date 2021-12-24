#ifndef UNIT_TEST

#include <Arduino.h>
#include <BleKeyboard.h>

#include <iostream>
#include <stdint.h>
#include <string>

#include "esp32-hal-cpu.h"
#include <esp_pm.h>

#include "config.hpp"
#include "event_manager.hpp"
#include "keyboard.hpp"
#include "mesh.hpp"

Config config = Config();
Keyboard keyboard = Keyboard(&config);
EventManager manager = EventManager();

// esp_pm_config_esp32 *config_pm;
void setup() {
  setCpuFrequencyMhz(240);
  pinMode(config.batt_pin, ANALOG);
  Serial.begin(config.baud_rate);

  keyboard.begin();
  manager.begin();

  keyboard.led->active_keys = &(keyboard.matrix->active_keys);

  printf("------------SPLITBOARD------------\n");
  // printf("layers size %d\n", keyboard.layers.size());
  printf("I am server? %d", keyboard.is_server);
}

void loop() {
  static bool ble_connected;

  keyboard.update();
  keyboard.led->update();
  // // manager.add_event("display");
  // if (keyboard.bluetooth.isConnected()) {
  //   // esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL0, ESP_PWR_LVL_N11);
  //   if (!(ble_connected)) {
  //     // manager.add_event("display");
  //   }

  //   ble_connected = 1;

  // } else {
  //   ble_connected = 0;
  // }
  // test_manager();
  // keyboard.led->update();

  // tmp stuff
}
#endif
