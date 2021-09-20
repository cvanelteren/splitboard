#ifndef UNIT_TEST
#include <WiFi.h>
#include <esp_now.h>

#include "event_manager.hpp"
#include <config.hpp>
#include <key_definitions.hpp>
#include <keyboard.hpp>
Config config = Config();
Keyboard keyboard = Keyboard(&config);

void (EventManager::*update)() = &EventManager::update;
EventManager manager = EventManager();

void setup() {
  manager.begin();

  // this needs to be here for some reason?
  WiFi.mode(WIFI_STA);
  Serial.begin(config.baud_rate);

  Serial.printf("------------SPLITBOARD------------\n");
  keyboard.begin();
  Serial.printf("layers size %d\n", keyboard.layers.size());
  // keyboard.wakeup();
  // keyboard.sleep();
  // pinMode(led_pin, OUTPUT);
  // Serial.printf("%d\n", keyboard.layers[0][2][1]);
  // The setup has to deal with weird casting of pointers
  // As such the setup has to be performed when an actual object is instantiated
  // It is therefore here
  // keyboard.rotaryEncoder->setup(
  //     [] { keyboard.rotaryEncoder->readEncoder_ISR(); },
  //     [] { keyboard.rotaryEncoder->onButtonClick(); });
  // keyboard.rotaryEncoder->enable();

  // keyboard.display->setFont(u8g2_font_tom_thumb_4x6_mf);

  //   esp_now_register_recv_cb(OnDataRecv);
  // }

  // keyboard.log->begin(*keyboard.display, 32, 10, &keyboard.fb[0]);
  // u8g2log.begin(*(keyboard.display), U8LOG_WIDTH, U8LOG_HEIGHT,
  // &keyboard.fb[0]); // connect to u8g2, assign buffer
  // u8g2log.setLineHeightOffset(0); // set extra space between lines in
  // pixel, this can be negative u8g2log.setRedrawMode(0); // 0: Update screen
  // with newline, 1: Update screen for every char
}

void loop() {

  // leds[0] = CRGB::Red;
  // FastLED.show();
  // delay(500);
  // leds[0] = CRGB::Black;
  // FastLED.show();
  // delay(500);
  // }

  // Print a number on the U8g2log window
  keyboard.display->setFont(u8g2_font_tom_thumb_4x6_mf);

  keyboard.update();
  // keyboard.led->serial_cycle();
  // keyboard.led->cycle();
  manager.add_event("display");
  // if (keyboard.bluetooth.isConnected()) {
  //   keyboard.bluetooth.print("A");

  //   delay(5000);
  // }
  // Serial.println("Wainting");
  // digitalWrite(led_pin, HIGH);
  // digitalWrite(led_pin, LOW);
}
#endif
