 // #include <Arduino.h>

// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLEUtils.h>

// #include <SPI.h>
// #include <U8g2lib.h>

// #include <driver/gpio.h>
// #include <driver/spi_master.h>

// // See the following for generating UUIDs:
// // https://www.uuidgenerator.net/

// #define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
// #define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// class MyCallbacks : public BLECharacteristicCallbacks {
//   void onWrite(BLECharacteristic *pCharacteristic) {
//     std::string value = pCharacteristic->getValue();

//     if (value.length() > 0) {
//       Serial.println("*********");
//       Serial.print("New value: ");
//       for (int i = 0; i < value.length(); i++)
//         Serial.print(value[i]);

//       Serial.println();
//       Serial.println("*********");
//     }
//   }
// };

// auto pin_sda = GPIO_NUM_4;
// auto pin_scl = GPIO_NUM_15;
// auto pin_reset = GPIO_NUM_16;
// // // auto display =
// // // u8g2(U8G2_R0, data = pin_sda, clock = pin_scl, reset = pin_reset);

// auto u8g2 =
//     U8G2_SSD1306_128X64_NONAME_1_SW_I2C(U8G2_R0, pin_scl, pin_sda,
//     pin_reset);

// u8g2_uint_t offset; // current offset for the scrolling text
// u8g2_uint_t width; // pixel width of the scrolling text (must be lesser than
// 128
//                    // unless U8G2_16BIT is defined
// const char *text = "Hallo Klootzakken "; // scroll this text from right to
// left
// // auto *font = u8g2_font_inb16_mr;
// auto *font = u8g2_font_7x14B_mr;

// void setup() {
//   Serial.begin(115200);

//   // Serial.println("1- Download and install an BLE scanner app in your
//   phone");
//   // Serial.println("2- Scan for BLE devices in the app");
//   // Serial.println("3- Connect to MyESP32");
//   // Serial.println(
//   //     "4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write
//   //     something");
//   // Serial.println("5- See the magic =)");

//   // BLEDevice::init("MyESP32");
//   // BLEServer *pServer = BLEDevice::createServer();

//   // BLEService *pService = pServer->createService(SERVICE_UUID);

//   // BLECharacteristic *pCharacteristic = pService->createCharacteristic(
//   //     CHARACTERISTIC_UUID,
//   //     BLECharacteristic::PROPERTY_READ |
//   BLECharacteristic::PROPERTY_WRITE);

//   // pCharacteristic->setCallbacks(new MyCallbacks());

//   // pCharacteristic->setValue("Hello World");
//   // pService->start();

//   // BLEAdvertising *pAdvertising = pServer->getAdvertising();
//   // pAdvertising->start();

//   u8g2.begin();

//   u8g2.setFont(font); // set the target font to calculate the pixel width
//   width = u8g2.getUTF8Width(text); // calculate the pixel width of the text

//   u8g2.setFontMode(0); // enable transparent mode, which is faster
// }

// void draw_text() {
//   u8g2_uint_t x;

//   u8g2.firstPage();
//   do {

//     // draw the scrolling text at current offset
//     x = offset;
//     u8g2.setFont(font);           // set the target font
//     do {                          // repeated drawing of the scrolling
//     text...
//       u8g2.drawUTF8(x, 30, text); // draw the scolling text
//       x += width;                 // add the pixel width of the scrolling
//       text
//     } while (x < u8g2.getDisplayWidth()); // draw again until the complete
//                                           // display is filled

//     u8g2.setFont(font); // draw the current pixel width
//     u8g2.setCursor(0, 58);
//     u8g2.print(
//         width); // this value must be lesser than 128 unless U8G2_16BIT is
//         set

//   } while (u8g2.nextPage());

//   offset -= 5; // scroll by one pixel
//   if ((u8g2_uint_t)offset < (u8g2_uint_t)-width)
//     offset = 0; // start over again
// }
// void loop(void) {
//   size_t frame = 0;
//   size_t fs = 40;

//   if (millis() - frame >= fs) {
//     frame = millis();
//     u8g2.clearBuffer();
//     draw_text();
//     u8g2.sendBuffer();
//   }
// }
