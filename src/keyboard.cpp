#include "keyboard.hpp"
#include <WiFi.h>

#include "driver/rtc_io.h"

#define LT(layer, kc) (kc | LAYER_TAP | ((layer & 0xF) << 8))
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

  this->led = new LED(config);

  byte mac_addr[6];
  WiFi.macAddress(mac_addr);
  this->is_server = true;
  for (int idx = 0; idx < 6; idx++) {
    if (mac_addr[idx] != config->serv_add[idx]) {
      this->is_server = false;
      break;
    }
  }

  // default to client
  if (this->is_server) {
    Serial.println("Setting up bluetooth");
    // this->bluetooth =
    // new BleKeyboard(config->device_name, config->device_manufacturer,
    // this->get_battery_level());
    this->bluetooth = BleKeyboard(
        config->device_name, config->device_manufacturer, get_battery_level());
    // this->bluetooth = new Bluetooth(config);
  }

  layer_t qwerty = {{KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8,
                     KC_9, KC_0, KC_BSPC},
                    {KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I,
                     KC_O, KC_P, KC_OBRACKET, KC_CBRACKET},
                    {KC_TAB, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K,
                     KC_L, KC_COLON, KC_SQUOTE},
                    {KC_LSHIFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M,
                     KC_COMMA, KC_DOT, KC_SLASH, KC_RSHIFT},
                    {KC_NO, KC_NO, KC_NO, KC_ALT, KC_LCTL, KC_SPC, KC_ENTER,
                     KC_SUPER, KC_SUPER, KC_NO, KC_NO, KC_NO}};
  // // std::unordered_map
  // layer_t qwerty = {{KC_B, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8,
  // KC_9,
  //                    KC_0, KC_BSPC},
  //                   {KC_A, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I,
  //                   KC_O,
  //                    KC_P, KC_OBRACKET, KC_CBRACKET},
  //                   {KC_B, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K,
  //                   KC_L,
  //                    KC_COLON, KC_SQUOTE},
  //                   {KC_C, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M,
  //                   KC_COMMA,
  //                    KC_DOT, KC_SLASH, KC_RSHIFT},
  //                   {KC_NO, KC_NO, KC_NO, KC_LCTL, KC_ALT, KC_SPC, KC_ENTER,
  //                    KC_SUPER, KC_SUPER, KC_NO, KC_NO, KC_NO}};

  this->layers = {key_layers::qwerty};
  this->layers = {qwerty};
  this->active_layer = &(this->layers[0]);

  // setup rotary encoder
  this->rotary_encoder = new RotaryEncoder(config);
  this->encoder_codes = {
      {"LEFT",
       {{"UP", &KEY_MEDIA_VOLUME_UP}, {"DOWN", &KEY_MEDIA_VOLUME_DOWN}}},
      {"RIGHT",
       {{"UP", &KEY_MEDIA_VOLUME_DOWN}, {"DOWN", &KEY_MEDIA_VOLUME_UP}}}};
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
  }
  Serial.println();

  this->mesh->begin();

  // setup start display
  if (this->display != nullptr) {
    this->display->begin();
    // FIXME: move this to display somehow
    auto width = 32;
    auto height = 10;

    this->display->log_buffer.resize(width * height);
    this->display->log.begin(*this->display, width, height,
                             &(this->display->log_buffer)[0]);
  }

  // start bluetooth when server
  if (this->is_server) {
    Serial.println("Starting bluetooth");
    this->bluetooth.begin();
    this->bluetooth.releaseAll();
  }

  if (this->led != nullptr) {
    this->led->begin();
  }

  if (this->rotary_encoder != NULL) {
    this->rotary_encoder->begin();
  }
}

// Function to toggle bits in the given range
int toggleBitsFromLToR(int n, int l, int r) {
  // calculating a number 'num' having 'r' number of bits
  // and bits in the range l to r are the only set bits
  int num = ((1 << r) - 1) ^ ((1 << (l - 1)) - 1);

  // toggle the bits in the range l to r in 'n'
  // and return the number
  return (n ^ num);
}

// Function to unset bits in the given range
int unsetBitsInGivenRange(int n, int l, int r) {
  // 'num' is the highest positive integer number
  // all the bits of 'num' are set
  long num = (1ll << (4 * 8 - 1)) - 1;

  // toggle the bits in the range l to r in 'num'
  num = toggleBitsFromLToR(num, l, r);

  // unset the bits in the range l to r in 'n'
  // and return the number
  return (n & num);
}

uint8_t Keyboard::read_key(keyswitch_t &keyswitch) {
  // encodes pin switch to actual char

  Serial.printf("row %d \t col %d (%d, %d)\t %d \n", keyswitch.row,
                keyswitch.col, keyswitch.source, keyswitch.sinc,
                keyswitch.time);
  if (keyswitch.pressed_down) {
    Serial.println("Key press");
  }
  // release event
  else {
    Serial.println("Key release");
  }

  auto keycode = (*this->active_layer)[keyswitch.col][keyswitch.row];
  size_t tmp;
  switch (unsetBitsInGivenRange(keycode, 12, 0)) {
  case KC_SLEEP: {
    if (keyswitch.pressed_down) {
      this->sleep();
    }
    break;
  }
  case (LAYER_TAP):
    // deal with layer tap stuff
    Serial.printf("Layer tap!");
    tmp = unsetBitsInGivenRange(keycode, 16, 12);
    tmp = unsetBitsInGivenRange(tmp, 8, 0) >> 8;
    // this->active_layer = tmp;
    break;
  default: {
    Serial.printf("Sending a keycode \n");
    return keycode;
    break;
  }
  }
}

void Keyboard::send_keys() {

  // read the mesh
  auto client_keys = Mesh::get_buffer();
  // Mesh::buffer.fill({});
  // read server keys
  auto &active_keys = this->matrix->active_keys;
  std::copy(client_keys.begin(), client_keys.end(),
            std::back_inserter(active_keys));

  // join the two lists
  uint8_t n_server_keys = active_keys.size();
  uint8_t total_keys = client_keys.size() + n_server_keys;

  if (total_keys) {
    Serial.printf("Active keys %d\n", total_keys);
    Serial.printf("Client keys %d\n", client_keys.size());
  }
  // read server active keys

  keyswitch_t *keyswitch;
  uint8_t key;
  const MediaKeyReport *encoder_code;
  for (int idx = 0; idx < total_keys; idx++) {
    // read switch
    keyswitch = &(idx < active_keys.size() ? active_keys[idx]
                                           : client_keys[idx - n_server_keys]);
    // read encoder state
    Serial.printf("%d %d\n", keyswitch->source, keyswitch->sinc);
    if (keyswitch->source == config->rot_a_pin or
        keyswitch->source == config->rot_b_pin) {
      Serial.printf("Received encoder key!\n");
      // server encoder
      if (keyswitch->buffer == 1) {
        encoder_code =
            (idx < active_keys.size() ? this->encoder_codes["LEFT"]["UP"]
                                      : this->encoder_codes["RIGHT"]["DOWN"]);
      } else {
        encoder_code =
            (idx < active_keys.size() ? this->encoder_codes["LEFT"]["DOWN"]
                                      : this->encoder_codes["RIGHT"]["UP"]);
      }
      if (this->bluetooth.isConnected()) {
        bluetooth.write(*encoder_code);
      }
    }
    // read keycode
    else {
      key = this->read_key(*keyswitch);
      if (this->bluetooth.isConnected()) {
        if (keyswitch->pressed_down) {
          Serial.printf("BLE send\n");
          this->bluetooth.press(key);
          // this->bluetooth->write(97);
        } else {
          Serial.printf("BLE release\n");
          this->bluetooth.release(key);
        }
      }
    }
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

  // bool state;
  // if (millis() - last_led_time > 1000) {
  //   pinMode(2, OUTPUT);
  //   state = (digitalRead(2) ? LOW : HIGH);
  //   digitalWrite(2, state);
  //   Serial.printf("Writing state as %d \n", state);
  //   last_led_time = millis();
  // }

  // update components
  this->matrix->update();
  this->rotary_encoder->update();
  // add rotary encoder key to matrix buffer
  for (auto elem : this->rotary_encoder->active_keys) {
    this->matrix->active_keys.push_back(elem);
  }

  if (this->matrix->active_keys.size()) {
    this->last_activity = millis();
  }
  // Serial.printf("%d %d\n", last_activity, millis() - last_activity);

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

  static bool ble_connected;
  if (this->bluetooth.isConnected()) {
    ble_connected = 1;
    // this->display->log.print("\rhello:)");
    // this->display->clearBuffer();
    // this->display->log.print("\rhello:)");
    // this->display->sendBuffer();

    // // this->display->firstPage();
    //   do {
    //     this->display->log.println("");
    //     this->display->log.print("\rhello:)");
    //     // this->display->log.print(printf("Connected to %s", "test"));
    //     // this->display->setFont(font);
    //     // this->display->drawUTF8(1, 30, "hello :)");

    //   } while (this->display->nextPage());
    // } else {
    //   // this->display->clearDisplay();
    //   this->display->firstPage();
    //   do {
    //     // this->display->log.println("");
    //     this->display->log.print("\rNo Bluetooth :(");
    //     // this->display->setFont(font);
    //     // this->display->drawUTF8(1, 30, "No bluetooth :(");

    //   }
    // while (this->display->nextPage())
    // ;
  } else {
    ble_connected = 0;
    // this->display->clearBuffer();
    // this->display->log.print("\rNo BLE :(");
    // this->display->sendBuffer();
  }

  this->led->ble_status(ble_connected);
}

void Keyboard::wakeup() {
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

  this->matrix->wakeup();
  this->led->wakeup();
  this->display->wakeup();
  this->last_activity = millis();
}

size_t Keyboard::get_last_activity() { return this->last_activity; }

void sleep_all_pin() {
  // tmp function to set all pins to low before sleeping
  auto pins = {0,  2,  4,  5,  12, 13, 14, 15, 16, 17, 18, 19, 21,
               22, 23, 25, 26, 27, 32, 33, 34, 36, 37, 38, 39};
  for (auto pin : pins) {
    Serial.printf("Turning %d off\n", pin);
    pinMode(pin, INPUT);
    digitalWrite(pin, LOW);
  }
}
void Keyboard::sleep() {
  /**
   * @brief      Enter deep sleep
   */
  this->led->sleep();
  // prepare pins for sleep
  this->matrix->sleep();
  this->display->sleep();

  esp_sleep_enable_touchpad_wakeup();
  Serial.println("Going sleepy time!");
  esp_light_sleep_start();

  // for deep sleep hold the energy state
  // gpio_deep_sleep_hold_en();
  // esp_deep_sleep_start();
  Serial.println("Sleeping"); // shouldn't print
  this->wakeup();
}
