
#include <BleKeyboard.h>

#include "driver/rtc_io.h"
#include "keyboard.hpp"
#include <WiFi.h>

#include <unordered_map>

Keyboard::~Keyboard() {
  delete config;
  delete matrix;
  delete mesh;
  delete display;
  delete manager;
};

Keyboard::Keyboard(Config *config) {
  this->config = config;
  // init the keyboard
  matrix = new Matrix(config);
  mesh = new Mesh(config);
  // setup display
  display = new Display(config);
  led = new LED(config);
  manager = new EventManager();

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

#include "esp_adc_cal.h"
extern Config config;
double read_voltage() {
  float calibration = 1.01;
  float vref = 1100;
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100,
                           &adc_chars);
  vref = adc_chars.vref;
  return (analogRead(config.batt_pin) / 4096.0) * 3.3 * (1100.0 / vref) *
         calibration;
}
double Keyboard::get_battery_level() {

  // All ADC pins have a 12 bit (4096) resolution. That is,
  // the output for reads will be an integer between 0-4095.
  // The max current on each pin is 3.3 volt. The 4095th bit, therefore,
  // represents 100 percent and 3.3v. Using a voltage divider I can convert
  // the 3.7V battery to within the 3.3V range and convert the output.
  static size_t last_time;
  // the range is 0.5V and the 2.0 is for the voltage dividor.

  // this is the efuse "corrected" at the factory
  // the db is necessary for the range 350mV -2250 mV
  // esp_adc_cal_characteristics_t adc_chars;
  // esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12,
  // 1100,
  //                          &adc_chars);
  // double correction = adc_chars.vref;
  // printf("%0.2f correction factor \n", correction);
  double voltage = analogRead(config->batt_pin) / 4096.0;
  double level = (voltage - BAT_MIN_ADC) / (BAT_MAX_ADC - BAT_MIN_ADC);

  // change this to correct range. This assumes lipo battery
  // double correction = ((3.7 - 3.2) / 3.3) * 4096.0;
  if ((millis() - last_time) > 100) {
    printf("Battery level: %0.2f %% %0.2fV \t%d %f %f \n", level, voltage,
           analogRead(config->batt_pin), BAT_MAX_ADC, BAT_MIN_ADC);
    last_time = millis();
  }

  return level;
}

void Keyboard::begin() {
  Serial.println("Starting keyboard");
  Serial.println(printf("Am I a server? %d\n", this->is_server));
  Serial.print("MAC address is: ");
  for (auto hex_num : WiFi.macAddress()) {
    Serial.print(hex_num);
  }
  Serial.println();

  led->begin();
  mesh->begin();
  rotary_encoder->begin();
  manager->begin();

  // setup start display
  if (display != NULL) {
    display->begin();
    // FIXME: move this to display somehow
    auto width = 32;
    auto height = 10;

    display->log_buffer.resize(width * height);
    display->log.begin(*this->display, width, height,
                       &(this->display->log_buffer)[0]);
  }

  // start bluetooth when server
  if (this->is_server) {
    printf("Starting bluetooth\n");
    bluetooth.begin();
    bluetooth.releaseAll();
  }
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
   * @details    keywsitches map to a keycode. Some of the keycodes are
   *special. That is, the allow for temporary switching to another layer when
   *a key is pressed, etc. This function deals with the various different
   *functions. It is highly likely that I will separate these into different
   *events.
   * @return     return type
   */

  // TODO: This  function needs to  be replaced with  a more
  // substantial method
  //
  // TODO: there  is a bug that switching
  // layers  with   stacked  layer  taps  will   not  return
  // properely to  the original layer. That  is, tapping one
  // layer tap from 0 -> 1 and  then on layer 1 a tap from 1
  // -> 2, will  have a  sequence effect where  depending on
  // how the keys are released will return to either layer 1
  // or 0
  //
  static std::unordered_map<uint8_t, uint8_t> layer_tracer;
  static std::unordered_map<uint8_t, uint8_t> modifier_tracker;

  uint16_t keycode = (*active_layer)[keyswitch.col][keyswitch.row];
  bool add_special_key;
  uint16_t layer, modifier;

  // read special keycodes
  switch ((keycode >> 12)) {
  case (MOD_TAP): {
    if (add_special) {
      this->keep_active(keyswitch);
    }
    // send modifier
    else if ((keyswitch.pressed_down) &
             (millis() - keyswitch.time) >= LAYER_TAP_DELAY_MS) {
      // identify the modifier and shift it back to the correct
      // range (> 128)
      modifier = ((keycode >> 8) & 0xF) | 0x80;
      if (!modifier_tracker[modifier]) {
        this->bluetooth.press(modifier);
        modifier_tracker[modifier] = true;
      }

    }
    // release event
    else if (!keyswitch.pressed_down) {
      // send tapped key
      if ((millis() - keyswitch.time) < LAYER_TAP_DELAY_MS) {
        this->bluetooth.press(keycode & 0xFF);
        this->bluetooth.release(keycode & 0xFF);
      }
      // release modifier
      else {
        modifier = ((keycode >> 8) & 0xF) | 0x80;
        this->bluetooth.release(modifier);
        modifier_tracker.erase(modifier);
      }
    }
    break;
  }
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
        // toggle_active_layer = active_layer_num;
        layer_tracer[layer] = active_layer_num;
        set_active_layer(layer);
        // set layer_tap info to keycode in higher layer
        (*active_layer)[keyswitch.col][keyswitch.row] =
            (keycode | (LAYER_TAP << 12));
      }
    }
    // key release event
    else if (!keyswitch.pressed_down) {
      printf("Layer tap release!\n");

      // reset the keycode; NOTE this prevents keycode from being pressed
      // twice?
      // (*active_layer)[keyswitch.col][keyswitch.row] &= ~(LAYER_TAP);
      // set_active_layer(toggle_active_layer);
      auto tmp = active_layer_num;
      set_active_layer(layer_tracer[active_layer_num]);
      layer_tracer.erase(tmp);
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
    break;
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
    default:
      if (keyswitch.pressed_down) {
        Serial.printf("BLE send\n");
        this->bluetooth.press(keycode);
      } else {
        Serial.printf("BLE release\n");
        this->bluetooth.release(keycode);
      }
    }
  }
  }
}

void Keyboard::process_special_keycodes() {
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
   *;
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
  matrix->wakeup();
  mesh->wakeup();
  led->wakeup();
  display->wakeup();
  last_activity = millis();
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
  led->sleep();
  // prepare pins for sleep
  mesh->sleep();
  matrix->sleep();
  display->sleep();

  esp_sleep_enable_touchpad_wakeup();
  Serial.println("Going sleepy time!");
  esp_light_sleep_start();

  // for deep sleep hold the energy state
  // gpio_deep_sleep_hold_en();
  // esp_deep_sleep_start();
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
