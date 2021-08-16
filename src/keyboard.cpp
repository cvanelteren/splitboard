#include "keyboard.hpp"
#include <WiFi.h>

#include "driver/rtc_io.h"

Keyboard::Keyboard(Config *config) {

  this->config = config;
  // init the keyboard
  this->matrix = new Matrix(config);
  // this->layout = new Layout(config);

  // setup esp_now
  // setting station wifi mode
  this->mesh = new Mesh(config);

  // setup display
  this->display = new Display(config);

  // default to client
  this->is_server = false;
  if (WiFi.macAddress() == config->server_address) {
    Serial.println("Setting up bluetooth");
    // this->bluetooth =
    // new BleKeyboard(config->device_name, config->device_manufacturer,
    // this->get_battery_level());
    this->bluetooth =
        BleKeyboard(config->device_name, config->device_manufacturer,
                    this->get_battery_level());
    // this->bluetooth = new Bluetooth(config);
    this->is_server = true;
  }

  // layer_t qwerty = {{KC_TILDE, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7,
  // KC_8,
  //                    KC_9, KC_0, KC_BSPC},
  //                   {KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I,
  //                    KC_O, KC_P, KC_OBRACKET, KC_CBRACKET},
  //                   {KC_TAB, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K,
  //                    KC_L, KC_COLON, KC_SQUOTE},
  //                   {KC_LSHIFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M,
  //                    KC_COMMA, KC_DOT, KC_SLASH, KC_RSHIFT},
  //                   {KC_NO, KC_NO, KC_NO, KC_LCTL, KC_ALT, KC_SPC, KC_ENTER,
  //                    KC_SUPER, KC_SUPER, KC_NO, KC_NO, KC_NO}};

  layer_t qwerty = {{KC_TILDE, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8,
                     KC_9, KC_0, KC_BSPC},
                    {KC_A, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O,
                     KC_P, KC_OBRACKET, KC_CBRACKET},
                    {KC_B, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L,
                     KC_COLON, KC_SQUOTE},
                    {KC_C, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMMA,
                     KC_DOT, KC_SLASH, KC_RSHIFT},
                    {KC_NO, KC_NO, KC_NO, KC_LCTL, KC_ALT, KC_SPC, KC_ENTER,
                     KC_SUPER, KC_SUPER, KC_NO, KC_NO, KC_NO}};

  this->layers = {key_layers::qwerty};
  this->layers = {qwerty};
  this->active_layer = &(this->layers[0]);

  // setup rotary encoder
  this->rotaryEncoder = new RotaryEncoder(config);
  // this->ble = BleKeyboard(config->name);

  // this->fb = std::vector<uint8_t>(this->display->getDisplayHeight() *
  // this->display->getDisplayWidth());

  // this->fb = std::vector<uint8_t>(64 / 7 * 128 / 14);
}

auto *font = u8g2_font_7x14B_mr;
// start the keyboard
//

double Keyboard::get_battery_level() { return 0.0; }
void Keyboard::begin() {

  // print info on bluetooth
  Serial.println("Starting keyboard");
  Serial.println(printf("Am I a server? %d\n", this->is_server));
  Serial.print("MAC address is: ");
  for (auto hex_num : WiFi.macAddress()) {
    Serial.print(hex_num);
    Serial.print(" ");
  }
  Serial.println();

  this->mesh->begin();

  // setup start display
  this->display->begin();

  // FIXME: move this to display somehow
  auto width = 32;
  auto height = 10;

  this->display->log_buffer.resize(width * height);
  this->display->log.begin(*this->display, width, height,
                           &(this->display->log_buffer)[0]);
  this->display->log.setRedrawMode(0);

  // start bluetooth when server
  if (this->is_server) {
    Serial.println("Starting bluetooth");
    this->bluetooth.begin();
    this->bluetooth.releaseAll();
  }

  // if (this->rotaryEncoder != NULL) {
  this->rotaryEncoder->begin();
  // }
  // this->display->setFont(u8g2_font_tom_thumb_4x6_mf);
  // this->display->setFont(u8g2_font_7x14B_mr);
  // this->display->setFont(u8g2_font_7x14_tf);

  // this->log->begin(*this->display, 64 / 7, 128 / 14, &this->fb[0]);

  // this->log->begin(*this->display, 8, 4, &this->fb[0]);

  // this->log->begin(8, 4, &this->fb[0]);

  // this->log->setLineHeightOffset(5);
  // this->log->setRedrawMode(1);
}

uint8_t Keyboard::read_key(keyswitch_t &keyswitch) {
  // encodes pin switch to actual char

  Serial.printf("row %d \t col %d (%d, %d)\t %d \n", keyswitch.row,
                keyswitch.col, keyswitch.source, keyswitch.sinc,
                keyswitch.time);
  if (keyswitch.active) {
    Serial.println("Key press");
  }
  // release event
  else {
    Serial.println("Key release");
  }

  // uint8_t col = (this->is_server ? keyswitch.col
  // : keyswitch.col + this->matrix->get_cols());
  // uint8_t row = keyswitch.row;
  // if (keyswitch.active)

  if (keyswitch.source == 21 && keyswitch.sinc == 13) {
    // only on key release
    if (!keyswitch.active)
      this->sleep();
  }

  if (keyswitch.source == 21 && keyswitch.sinc == 26) {
    Serial.println("SENDING SHIFT");
    // return SHIFT + 1;
  } else {
    // return (*this->active_layer)[2][1]; // qwerty a
    // Serial.printf("%d\n",
    // (*this->active_layer)[keyswitch.row][keyswitch.col]);
    return (*this->active_layer)[keyswitch.row][keyswitch.col];
  }
}

void Keyboard::send_keys() {

  // read the mesh
  auto client_keys = Mesh::get_buffer();
  // Mesh::buffer.fill({});
  // read server keys
  auto &active_keys = this->matrix->active_keys;

  // join the two lists
  uint8_t n_server_keys = active_keys.size();
  uint8_t total_keys = client_keys.size() + n_server_keys;

  // init report & counter
  // uint8_t key;
  keyswitch_t *keyswitch;
  uint8_t key;

  // read server active keys

  if (total_keys) {
    Serial.printf("Active keys %d\n", total_keys);
    Serial.printf("Client keys %d\n", client_keys.size());
  }

  for (int idx = 0; idx < total_keys; idx++) {
    // read switch
    keyswitch = &(idx < active_keys.size() ? active_keys[idx]
                                           : client_keys[idx - n_server_keys]);
    key = this->read_key(*keyswitch);

    if (this->bluetooth.isConnected()) {
      if (keyswitch->active) {
        Serial.printf("BLE send\n");
        this->bluetooth.press(key);
        // this->bluetooth->write(97);
      } else {
        Serial.printf("BLE release\n");
        this->bluetooth.release(key);
      }
    }
  }

  // send encoder
  // FIXME: mv into nice interface
  // for (auto &elem : this->rotaryEncoder->get_keys()) {
  //   Serial.printf("Writing vol %d \n", elem);
  //   if (elem > 0) {
  //     this->bluetooth->write(KEY_MEDIA_VOLUME_UP);
  //   } else {
  //     this->bluetooth->write(KEY_MEDIA_VOLUME_DOWN);
  //   }
  //   total_keys++;
  // }

  if (total_keys) {
    this->last_activity = millis();
  }
}

void Keyboard::update() {
  /**
   * @brief     keyboard updater
   * @details  Main controller  for keyboard  functionality.
   * Reads switch states and pushes it to bluetooth.
   *
   */
  // Serial.println("Scanning");

  // update components
  this->matrix->update();
  this->rotaryEncoder->update();

  // handle sending keys
  // handle server
  if (this->is_server) {
    this->send_keys();
  }
  // handle client
  else {
    if (this->matrix->active_keys.size()) {
      this->mesh->send(this->matrix->active_keys);
    }
    // delay(10);
  }

  // idle check check
  if ((millis() - this->get_last_activity()) >=
      this->config->deep_sleep_timeout) {
    this->sleep();
  }

  // delay(150);
}

void Keyboard::wake_up() {
  /**
   * @brief      Wake-up from deep sleep
   */

  esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();
  switch (reason) {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wake up from ext0");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wake up from ext1");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup touch signal");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup from ULP");
    break;
  default:
    Serial.println("Wakeup not caused by deep sleep");
    break;
  }

  this->matrix->setup_keys();

  // esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TOUCHPAD);
  // touch_pad_intr_disable();
  // // reinit the pins of the matrix
  // esp_bluedroid_enable();
  // esp_wifi_start();

  // this->matrix->setup_pin();
}

void callback(){
    // callback stuff
    // Serial.println("In touch callback");
};

size_t Keyboard::get_last_activity() { return this->last_activity; }
void Keyboard::sleep() {
  /**
   * @brief      Enter deep sleep
   */

  for (auto pin : this->config->col_pins) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    Serial.printf("%d %d \n", pin, touchRead(pin));
  }

  uint8_t threshold = 20;
  // IMPORTANT: set sleep pins high(!)
  for (auto pin : this->config->row_pins) {
    // mask += pow(2, pin);
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    // gpio_pullup_en((gpio_num_t)pin);
    // pinMode(pin, INPUT_PULLUP);
    // digitalWrite(pin, HIGH);
    // gpio_hold_en(GPIO_NUM_13);
    touchAttachInterrupt(pin, callback, threshold);
    // rtc_gpio_wakeup_enable((gpio_num_t)pin, GPIO_INTR_LOW_LEVEL);
    Serial.printf("%d %d \n", pin, touchRead(pin));
  }

  // for (auto pin : this->config->row_pins) {
  // esp_sleep_enable_ext0_wakeup((gpio_num_t)pin, LOW);
  // }

  // // // debug code
  // while (true) {
  //   for (auto pin : this->config->row_pins) {
  //     Serial.printf("%d %d %d\n", pin, digitalRead(pin), touchRead(pin));
  //   }
  //   delay(60);
  //   // delay(200);
  // }

  // Serial.printf("%d\n", mask);

  // // touch_pad_set_group_mask(mask, 0, 0);
  // touch_pad_intr_enable();
  //
  // pinMode(GPIO_NUM_12, OUTPUT);
  // digitalWrite(GPIO_NUM_12, 1);
  // Serial.printf("%d \n", digitalRead(GPIO_NUM_12));
  // esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 1);
  // esp_sleep_enable_ext1_wakeup(0x000C, ESP_EXT1_WAKEUP_ANY_HIGH);
  // gpio_deep_sleep_hold_en();
  // touch_pad_intr_enable();
  esp_sleep_enable_touchpad_wakeup();
  Serial.println("Going sleepy time!");
  esp_light_sleep_start();
  // esp_deep_sleep_start();
  Serial.println("Sleeping"); // shouldn't print
}
