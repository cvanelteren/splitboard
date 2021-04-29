/**
 * This example turns the ESP32 into a Bluetooth LE keyboard that writes the
 * words, presses Enter, presses a media key and then Ctrl+Alt+Delete
 */

#include <Arduino.h>
#include <BleKeyboard.h>

#include <SPI.h>
#include <Wire.h>

#include "config.hpp"
#include "keyboard.hpp"

Config config = Config();
Keyboard keyboard = Keyboard(&config);
auto ble = BleKeyboard("Test");

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  Serial.println("1- Download and install an BLE scanner app in your phone");
  Serial.println("2- Scan for BLE devices in the app");
  Serial.println("3- Connect to MyESP32");
  Serial.println(
      "4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
  Serial.println("5- See the magic =)");
  keyboard.begin();
}

auto *font = u8g2_font_7x14B_mr;
void loop() {
  if (keyboard.ble.isConnected()) {
    // Serial.println("Sending 'Hello world'...");
    keyboard.display->firstPage();
    do {
      keyboard.display->setFont(font);
      keyboard.display->drawUTF8(1, 30, "hello from bluetooth");

    } while (keyboard.display->nextPage());
    delay(1000);
    // Serial.println("Sending 'Hello world'...");
    // bleKeyboard.print("Hello world");

    // delay(1000);

    // Serial.println("Sending Enter key...");
    // bleKeyboard.write(KEY_RETURN);

    // delay(1000);

    // Serial.println("Sending Play/Pause media key...");
    // bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);

    // delay(1000);

    // bleKeyboard.press(KEY_LEFT_CTRL);
    // bleKeyboard.press(KEY_LEFT_ALT);
    // delay(100);
    // bleKeyboard.releaseAll();
  } else {
    keyboard.display->clearDisplay();
    do {
      keyboard.display->setFont(font);
      keyboard.display->drawUTF8(1, 30, "No bluetooth :(");

    } while (keyboard.display->nextPage());
    // Serial.println("Waiting...");
    delay(1000);
  }
  // delay(5000);
}
