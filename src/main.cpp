#ifndef UNIT_TEST
#include <WiFi.h>
#include <esp_now.h>
#include <string>

#include "event_manager.hpp"
#include <config.hpp>
#include <key_definitions.hpp>
#include <keyboard.hpp>
Config config = Config();
Keyboard keyboard = Keyboard(&config);
EventManager manager = EventManager();

void setup() {
  // this needs to be here for some reason?
  WiFi.mode(WIFI_STA);
  Serial.begin(config.baud_rate);

  keyboard.begin();
  manager.begin();

  Serial.printf("------------SPLITBOARD------------\n");
  Serial.printf("layers size %d\n", keyboard.layers.size());

  // debug stuff
  // keyboard.wakeup();
  // keyboard.sleep();
}

static uint8_t counter;
void loop() {
  keyboard.update();
  // keyboard.led->update();

  // tmp stuff
  // printf("Counter \t %d \n", counter);
  if (counter == 0x0) {
    counter = 1;
    manager.add_event(std::string("display"));
  } else {
    manager.add_event(std::string("led"));
  }
  counter <<= 1;
}
#endif
