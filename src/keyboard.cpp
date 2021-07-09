#include "keyboard.hpp"

#include <Arduino.h>

#include <WiFi.h>

#include "mesh.hpp"

#include "types.h"
Keyboard::Keyboard(Config *config) {
  // setting station wifi mode
  // for esp_now

  Serial.println("Setting up keyboard");
  // init the keyboard
  this->matrix = new Matrix(config);
  // this->layout = new Layout(config);

  this->layers = {{1, config->test}};

  // setup esp_now
  Serial.println("Setting up mesh connection");
  this->mesh = new Mesh(config);

  // setup display
  Serial.println("Setting up display");
  this->display = new Display(config);

  // default to client
  this->is_server = false;
  if (WiFi.macAddress() == config->server_address) {
    Serial.println("Setting up bluetooth");
    this->bluetooth = new Bluetooth(config);
    this->is_server = true;
  }

  // this->ble = BleKeyboard(config->name);

  // this->fb = std::vector<uint8_t>(this->display->getDisplayHeight() *
  // this->display->getDisplayWidth());

  // this->fb = std::vector<uint8_t>(64 / 7 * 128 / 14);
}

auto *font = u8g2_font_7x14B_mr;
// start the keyboard
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

  // move this to display somehow
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

  // this->display->setFont(u8g2_font_tom_thumb_4x6_mf);
  // this->display->setFont(u8g2_font_7x14B_mr);
  // this->display->setFont(u8g2_font_7x14_tf);

  // this->log->begin(*this->display, 64 / 7, 128 / 14, &this->fb[0]);

  // this->log->begin(*this->display, 8, 4, &this->fb[0]);

  // this->log->begin(8, 4, &this->fb[0]);

  // this->log->setLineHeightOffset(5);
  // this->log->setRedrawMode(1);
}

void Keyboard::send_keys() {
  // read the mesh
  // send the keys
}
void Keyboard::update() {
  // Serial.println("Scanning");
  this->matrix->update();

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

      for (int i = 0; i < this->matrix->active_keys.size(); i++) {
        Serial.println("Adding keys");
        this->mesh->buffer[i % 5] = this->matrix->active_keys[i];
        if (i > 4) {
          this->mesh->send();
        }
        if (i == this->matrix->active_keys.size() - 1) {
          this->mesh->send();
          this->mesh->buffer.fill({});
        }
      }
      this->mesh->send();
    }

    // this->mesh->send(KeyData{this->matrix->active_keys});
    // this->display->log.print("\rI am a client");
  }
  // delay(10);
}
