#ifndef UNIT_TEST
#include "esp32-hal-cpu.h"
#include <WiFi.h>
#include <esp_now.h>
#include <string>

#include "event_manager.hpp"
#include <config.hpp>
#include <esp_pm.h>
#include <key_definitions.hpp>
#include <keyboard.hpp>
Config config = Config();
Keyboard keyboard = Keyboard(&config);
EventManager manager = EventManager();

// esp_pm_config_esp32 *config_pm;
#include <esp_bt.h>
void setup() {
  // setCpuFrequencyMhz(240);
  pinMode(config.batt_pin, ANALOG);
  setCpuFrequencyMhz(240);
  // esp_pm_get_configuration(config_pm);

  // this needs to be here for some reason?
  WiFi.mode(WIFI_STA);
  Serial.begin(config.baud_rate);

  keyboard.begin();
  manager.begin();
  //
  keyboard.led->active_keys = &(keyboard.matrix->active_keys);
  // keyboard.sleep();

  Serial.printf("------------SPLITBOARD------------\n");
  Serial.printf("layers size %d\n", keyboard.layers.size());

  // keyboard.display->sleep();
  // keyboard.mesh->end();

  // esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P1);
  // esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_N2);

  // esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P1);

  // keyboard.sleep();
  // keyboard.display->sleep();

  // debug stuff
  // keyboard.wakeup();
  // keyboard.sleep();
}
// esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P1);

void test_manager() {
  static uint8_t counter;
  // printf("Counter \t %d \n", counter);
  if (counter == 0x0) {
    counter = 1;
    manager.add_event(std::string("display"));
  } else {

    manager.add_event(std::string("led"));
  }
  counter <<= 1;
  // printf("Counter :%d\n", counter);
}
void loop() {
  static bool ble_connected;

  keyboard.update();

  if (keyboard.bluetooth.isConnected()) {
    // esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL0, ESP_PWR_LVL_N11);
    ble_connected = 1;
  } else {
    ble_connected = 0;
  }
  manager.add_event("display");
  // test_manager();
  // keyboard.led->update();

  // tmp stuff
}
#endif
