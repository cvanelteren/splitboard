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
    this->bluetooth = BleKeyboard(
        config->device_name, config->device_manufacturer, get_battery_level());
  }

  layer_t qwerty = {{KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8,
                     KC_9, KC_0, KC_BSPC},
                    {KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I,
                     KC_O, KC_P, KC_LCBRACKET, KC_RCBRACKET},
                    {KC_TAB, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K,
                     KC_L, KC_SCOLON, KC_SQUOTE},
                    {KC_LSHIFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M,
                     KC_COMMA, KC_DOT, KC_SLASH, KC_RSHIFT},
                    {KC_NO, KC_NO, KC_NO, KC_ALT, KC_LCTL, KC_SPC,
                     LT(1, KC_ENTER), KC_SUPER, LT(1, KC_ALTGR), KC_NO, KC_NO,
                     KC_NO}};
  {} {}
  layer_t program = {{KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8,
                      KC_9, KC_0, KC_BSPC},
                     {
                         KC_TAB,
                         KC_Q,
                         KC_W,
                         KC_LCBRACKET,
                         KC_RCBRACKET,
                         KC_PLUS,
                         KC_Y,
                         KC_U,
                         KC_I,
                         KC_O,
                         KC_LBRACKET,
                         KC_RBRACKET,
                     },
                     {KC_TAB, KC_K, KC_S, KC_LBRACKET, KC_RBRACKET, KC_EQ,
                      KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, KC_SCOLON, KC_SQUOTE},
                     {KC_LSHIFT, KC_Z, KC_X, KC_C, KC_V, KC_UNDERSCORE, KC_N,
                      KC_M, KC_COMMA, KC_DOT, KC_SLASH, KC_RSHIFT},
                     {KC_NO, KC_NO, KC_NO, KC_ALT, KC_TRNS, KC_SPC,
                      LT(1, KC_ENTER), KC_SUPER, KC_SUPER, KC_NO, KC_NO,
                      KC_NO}};

  this->layers = {qwerty, program};
  // this->layers.push_back(qwerty);
  // this->layers.push_back(qwerty);
  this->set_active_layer(0);

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
  Serial.println("Starting keyboard");
  Serial.println(printf("Am I a server? %d\n", this->is_server));
  Serial.print("MAC address is: ");
  for (auto hex_num : WiFi.macAddress()) {
    Serial.print(hex_num);
  }
  Serial.println();

  this->led->begin();
  this->mesh->begin();
  this->rotary_encoder->begin();

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

void Keyboard::keep_active(keyswitch_t &keyswitch) {
  /**
   * @brief add keyswitch to  the special keycode buffer for
   * processing layer taps, leader key etc
   */
  bool add_special_key = true;
  // copy press down or release evet in the prevent buffer
  for (auto &elem : special_keycodes) {
    if ((elem.sinc == keyswitch.sinc) && (elem.source == keyswitch.source)) {
      elem.pressed_down = keyswitch.pressed_down;
      add_special_key = false;
    }
  }
  if (add_special_key) {
    keyswitch.time = millis();
    special_keycodes.push_back(keyswitch);
  }
}

// TODO: this needs a cleaner interface..
// perhaps a separate event class
void Keyboard::process_keyswitch(keyswitch_t &keyswitch, bool add_special = 0) {
  /**
   * @brief      Process the keyswitches
   *
   * @details    keywsitches map to a keycode. Some of the keycodes are special.
   * That is, the allow for temporary switching to another layer when a key is
   *pressed, etc. This function deals with the various different functions.
   *It is highly likely that I will separate these into different events.
   * @return     return type
   */
  static uint8_t toggle_active_layer; // remember the past active layer
  uint16_t keycode = (*active_layer)[keyswitch.col][keyswitch.row];
  bool add_special_key;
  uint16_t layer;

  // read special keycodes
  switch ((keycode >> 12) << 12) {
  case (MOD_TAP): {
    if (add_special) {
      this->keep_active(keyswitch);
    }
    // send modifier
    else if ((keyswitch.pressed_down) &
             (millis() - keyswitch.time) >= LAYER_TAP_DELAY_MS) {
      //

    }
    // release event
    else if (!keyswitch.pressed_down) {
      // release both keys?
    }
  }

  // TODO: move this to separate function
  case (LAYER_TAP): {
    // deal with layer tap stuff
    // Serial.printf("Layer tap!\n");
    // adding event
    if (add_special) {
      this->keep_active(keyswitch);
    }
    // key press event
    else if ((keyswitch.pressed_down) &
             ((millis() - keyswitch.time) >= LAYER_TAP_DELAY_MS)) {
      // switch layer
      layer = ((keycode >> 8) & 0xF);
      printf("Layer %d %d %d\r", layer, keycode >> 8, keycode);
      if (layer != active_layer_num) {
        Serial.printf("Layer tap holding\n");
        toggle_active_layer = active_layer_num;
        set_active_layer(layer);
        // set layer_tap info to keycode in higher layer
        (*active_layer)[keyswitch.col][keyswitch.row] = (keycode | LAYER_TAP);
      }
    }
    // key release event
    else if (!keyswitch.pressed_down) {
      printf("Layer tap release!\n");

      // reset the keycode; NOTE this prevents keycode from being pressed twice?
      // (*active_layer)[keyswitch.col][keyswitch.row] &= ~(LAYER_TAP);
      set_active_layer(toggle_active_layer);
      printf("Delta %d %d %d \n", (millis() - keyswitch.time), keyswitch.time,
             millis());
      if ((millis() - keyswitch.time) < LAYER_TAP_DELAY_MS) {
        this->bluetooth.press(keycode);
        this->bluetooth.release(keycode);
      }
      // TODO: if layer is released before key is released, the keycode is
      // This temporary fixes is
      else {
        this->bluetooth.releaseAll();
      }
    }
    break;
  }
  case ONESHOT: {
    printf("One shot pressed!\n");
    // outline:
    // before another key is pressed
    // ignore key release when one shot is pressed
    // control this with a static flag inside this function?
    // when second key is pressed release the modifier key pressed
  }
  default: {
    // TODO add KC_LEAD events
    Serial.printf("Sending a keycode \n");

    // deal non standard keycodes
    switch (keycode) {
    case (KC_TRNS): {
      // TODO
      break;
    }
    case KC_SLEEP: {
      if (keyswitch.pressed_down) {
        // TODO: send sleep message to other half
        this->sleep();
      }
      break;
    }
    }
    switch (this->bluetooth.isConnected()) {
      switch (keyswitch.pressed_down) {
      case 0:
        Serial.printf("BLE release\n");
        this->bluetooth.release(keycode);
        break;
      case 1:
        Serial.printf("BLE send\n");
        this->bluetooth.press(keycode);
        break;
      }
      break;
    }
  }
  }
}

void Keyboard::process_special_keycodes() {
  uint16_t keycode;
  keyswitch_t *keyswitch;

  // printf("Processing special keycodes\n");
  // printf("Num special keys: \t %d\n", special_keycodes.size());
  for (int idx = special_keycodes.size() - 1; idx >= 0; idx--) {
    keyswitch = &special_keycodes[idx];
    process_keyswitch(*keyswitch, 0);
    // event dealt with
    if (!keyswitch->pressed_down) {
      special_keycodes.erase(special_keycodes.begin() + idx);
    }
  }
}
void Keyboard::process_keyswitches() {
  // if non-empty deal with special keycodes first
  process_special_keycodes();
  // read the mesh
  auto client_keys = Mesh::get_buffer();
  // Mesh::buffer.fill({});
  // read server keys
  auto &active_keys = this->matrix->active_keys;

  // std::copy(client_keys.begin(), client_keys.end(),
  // std::back_inserter(active_keys));

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

    if (idx >= active_keys.size()) {
      // TODO: FIX THIS
      keyswitch->row = 11 - keyswitch->row;
    }
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
      this->process_keyswitch(*keyswitch, 1);
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
#ifdef USE_ENCODER
  this->rotary_encoder->update();
#endif
  // add rotary encoder key to matrix buffer
  for (auto elem : this->rotary_encoder->active_keys) {
    this->matrix->active_keys.push_back(elem);
  }

  // ensures each half remains active
  if (this->matrix->active_keys.size()) {
    this->last_activity = millis();
  }
  // Serial.printf("%d %d\n", last_activity, millis() - last_activity);

  // handle sending keys
  // handle server
  if (this->is_server) {
    this->process_keyswitches();
  }
  // handle client
  else {
    if (this->matrix->active_keys.size()) {
      this->mesh->send(this->matrix->active_keys);
    }
    // delay(10);
  }

#ifdef USE_SLEEP
  // // idle check check
  if ((millis() - this->get_last_activity()) >=
      this->config->deep_sleep_timeout) {
    this->sleep();
  }
#endif

  static bool ble_connected;
  if (this->bluetooth.isConnected()) {
    if (bluetooth.isConnected() ^ ble_connected)
      printf("Bluetooth is connected\n");
    ble_connected = 1;
  } else {
    if (bluetooth.isConnected() ^ ble_connected)
      printf("Bluetooth disconnected\n");
    ble_connected = 0;
  }
  led->update();
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
  // esp_light_sleep_start();

  // for deep sleep hold the energy state
  // gpio_deep_sleep_hold_en();
  esp_deep_sleep_start();
  Serial.println("Sleeping"); // shouldn't print
  this->wakeup();
}

void Keyboard::set_active_layer(uint8_t layer) {
  if (layer < layers.size()) {
    printf("Switching to layer %d\n", layer);
    active_layer = &layers[layer];
    active_layer_num = layer;
  } else {
    printf("Warning setting layer larger than max number of layers\n");
  }
}
