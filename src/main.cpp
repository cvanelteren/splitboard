#ifndef UNIT_TEST

#define USE_NIMBLE
#include "esp32-hal-cpu.h"
#include <WiFi.h>
#include <esp_now.h>
#include <string>

#include "event_manager.hpp"
#include <config.hpp>
#include <esp_pm.h>
#include <key_definitions.hpp>
#include <keyboard.hpp>

// #include <BLEDevice.h>
// #include <BLEServer.h>

#include <esp_pm.h>
Config config = Config();
Keyboard keyboard = Keyboard(&config);
EventManager manager = EventManager();

BLEServer *server = BLEDevice::createServer();

// esp_pm_config_esp32 *config_pm;
void setup() {
  // setCpuFrequencyMhz(240);
  // pinMode(config.batt_pin, ANALOG);
  setCpuFrequencyMhz(240);
  // esp_pm_get_configuration(config_pm);

  // this needs to be here for some reason?
  WiFi.mode(WIFI_STA);
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
