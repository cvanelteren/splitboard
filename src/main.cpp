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

  Serial.printf("------------SPLITBOARD------------\n");
  keyboard.begin();
  Serial.printf("layers size %d\n", keyboard.layers.size());

  manager.begin();

  // keyboard.wakeup();
  // keyboard.sleep();
}

void loop() {

  // Print a number on the U8g2log window
  keyboard.display->setFont(u8g2_font_tom_thumb_4x6_mf);
  keyboard.update();
  // keyboard.led->update();

  static uint8_t counter;
  counter <<= 1;
  if (counter == 0x0) {
    manager.add_event(std::string("display"));
    counter += 1;
  } else
    manager.add_event(std::string("led"));
}
#endif
