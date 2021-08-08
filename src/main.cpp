
#include <WiFi.h>
#include <esp_now.h>

#include "config.hpp"
#include "key_definitions.hpp"
#include "keyboard.hpp"
Config config = Config();
Keyboard keyboard = Keyboard(&config);

RTC_DATA_ATTR int bootCount = 0;

void setup() {

  // this needs to be here for some reason?
  WiFi.mode(WIFI_STA);
  Serial.begin(config.baud_rate);
  Serial.println("Starting BLE work!");
  Serial.println("1- Download and install an BLE scanner app in your phone");
  Serial.println("2- Scan for BLE devices in the app");
  Serial.println("3- Connect to MyESP32");
  Serial.println(
      "4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
  Serial.println("5- See the magic =)");

  // Set device as a Wi-Fi Station

  // // Init ESP-NOW
  // if (esp_now_init() != ESP_OK) {
  //   Serial.println("Error initializing ESP-NOW");
  //   return;
  // }

  bootCount++;
  keyboard.begin();

  Serial.printf("layers size %d\n", keyboard.layers.size());

  keyboard.wake_up();
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
  // Print a number on the U8g2log window
  keyboard.display->setFont(u8g2_font_tom_thumb_4x6_mf);

  keyboard.update();
}
