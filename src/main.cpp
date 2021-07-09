
#include <WiFi.h>
#include <esp_now.h>

#include "config.hpp"
#include "keyboard.hpp"
Config config = Config();
Keyboard keyboard = Keyboard(&config);

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  String d;
  bool e;
} struct_message;

struct_message myData = {};
uint8_t broadcastAddress[6] = {0x80, 0x7D, 0x3A, 0xD4, 0x2C, 0x9C};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.a);
  Serial.print("Int: ");
  Serial.println(myData.b);
  Serial.print("Float: ");
  Serial.println(myData.c);
  Serial.print("String: ");
  Serial.println(myData.d);
  Serial.print("Bool: ");
  Serial.println(myData.e);
  Serial.println();
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success"
                                                : "Delivery Fail");
}

void setup() {

  Serial.begin(config.baud_rate);
  Serial.println("Starting BLE work!");
  Serial.println("1- Download and install an BLE scanner app in your phone");
  Serial.println("2- Scan for BLE devices in the app");
  Serial.println("3- Connect to MyESP32");
  Serial.println(
      "4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
  Serial.println("5- See the magic =)");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // // Init ESP-NOW
  // if (esp_now_init() != ESP_OK) {
  //   Serial.println("Error initializing ESP-NOW");
  //   return;
  // }

  keyboard.begin();
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

  // strcpy(myData.a, "THIS IS A CHAR");
  // myData.b = random(1, 20);
  // myData.c = 1.2;
  // myData.d = "Hello";
  // myData.e = false;

  // if (keyboard.is_server) {
  //   esp_err_t result =
  //       esp_now_send(config.client_add, (uint8_t *)&myData, sizeof(myData));
  //   if (result == ESP_OK) {
  //     Serial.println("Msg send correctly");
  //   } else {
  //     Serial.println("Msg failed");
  //   }
  // }

  // for (auto i = 0; i < 32; i++) {
  //   keyboard.display->log.print("this is a very long text line");
  //   keyboard.display->log.print("\n");
  // }

  // keyboard.display->clearDisplay();

  // for (auto i = 0; i < 64; i++) {
  //   keyboard.log->print(millis());
  //   keyboard.log->print(" ");
  // }

  // u8g2log.print(millis());
  // Print a new line, scroll the text window content if required
  // Refresh the screen

  // u8g2log.print("\n");

  // keyboard.update();
  // delay(500);
  // delay(5000);
}
