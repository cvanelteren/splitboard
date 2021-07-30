#include "keyboard.hpp"
#include <WiFi.h>

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
    this->bluetooth =
        new BleKeyboard(config->device_name, config->device_manufacturer,
                        this->get_battery_level());
    // this->bluetooth = new Bluetooth(config);
    this->is_server = true;
  }

  layers_t layers = {key_layers::qwerty};

  this->layers = layers;
  this->active_layer = &this->layers[0];

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
    this->bluetooth->begin();
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

  // uint8_t col = (this->is_server ? keyswitch.col
  // : keyswitch.col + this->matrix->get_cols());
  // uint8_t row = keyswitch.row;
  Serial.printf("row %d %col %d\n", keyswitch.source, keyswitch.sinc);
  if (keyswitch.source == 21 && keyswitch.sinc == 26) {
    Serial.println("SENDING SHIFT");
    return SHIFT + 1;
  } else
    return (*this->active_layer)[0][0];
  // return this->active_layer[keyswitch.row][col];
}

// void Keyboard::send_keys() {
//   // read the mesh
//   // send the keys
//   if (this->bluetooth->isConnected()) {
//     // this->bluetooth->print("Hello world");
//     if (Mesh::buffer[0].active) {
//       // send a
//       this->bluetooth->write(0x41);
//       Mesh::buffer.fill({});
//     }
//   }
// }

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

  // KeyReport keys = {};
  // uint8_t counter = 0;
  // uint8_t col;
  // read matrix state
  // read the keys from client(s)
  // merge the keys in a singular key report

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
    // add idx to columns
    // if (this->bluetooth->isConnected()) {
    // press event
    if (keyswitch->active) {
      Serial.println("Key press");
    }
    // release event
    else {
      Serial.println("Key release");

      // this->bluetooth->release(key);
    }

    if (this->bluetooth->isConnected()) {
      if (keyswitch->active)
        this->bluetooth->press(key);
      else
        this->bluetooth->release(key);
    }
    // }

    // if (counter > 5) {
    //   // send key report
    //   this->bluetooth->sendReport(keys);
    //   keys.keys.fill(0);
    //   counter = 0;
    // }

    // this->bluetooth->write(keys);
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
  this->matrix->update();

  this->rotaryEncoder->update();

  if (this->is_server) {
    this->send_keys();

    // this->mesh->buffer.active_keys = this->matrix->active_keys;
    // 1. collect message from client
    // 2. collect active keys
    // 3. merge the keys
    // 4. send through bluetooth
    // if (this->bluetooth->connection->connected) {

    //   this->display->firstPage();
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

    //   } while (this->display->nextPage());
    //   // delay(10);
    // }
  }
  // handle client
  else {
    // TODO move this into a separate class that deals with this?

    if (this->matrix->active_keys.size()) {
      // Mesh::buffer.active_keys.resize(this->matrix->active_keys.size());
      // memcpy(&Mesh::buffer.active_keys, &this->matrix->active_keys,
      // this->matrix->active_keys.size());
      // Mesh::buffer = this->matrix->active_keys;

      // Mesh::buffer.active_keys = this->matrix->active_keys;
      // this->mesh->send();

      this->mesh->send(this->matrix->active_keys);

      // this->mesh->send(KeyData{this->matrix->active_keys});
      // this->display->log.print("\rI am a client");
    }
    // delay(10);
  }
}
