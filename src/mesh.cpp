#include "mesh.hpp"

// TODO: write a keycode msg buffer
// remove the static class property
// add return types to the send functions
// needs functions on data received for server
// and for client; in this case let's focus only on the
// server

// logs if a message was sent correctly
// TODO
void handle_input(const unsigned char *addr, const uint8_t *data, int len){};

Mesh::Mesh() {
  // WiFi.mode(WIFI_MODE_STA);
  // initialize wifi before esp-now
}

Mesh::Mesh(Config *config) : Mesh() {
  // WiFi.mode(WIFI_STA);
  Serial.println("Setting up mesh connection");
  this->config = config;
}

void Mesh::add_peer(const uint8_t *peer_addr) {
  this->peer = {};
  memcpy(this->peer.peer_addr, peer_addr, 6);
  this->peer.channel = 0;
  this->peer.encrypt = false;

  if (esp_now_add_peer(&(this->peer)) != ESP_OK) {
    Serial.println("Failed to add peer");
  } else {
    Serial.println("Added peer");
  }
}

void Mesh::begin() {
  WiFi.mode(WIFI_STA);
  this->init_esp_now();
  // add peer

  uint8_t mac_addr[6];
  WiFi.macAddress(mac_addr);
  if (mac_addr == this->config->serv_add) {
    // init client stuff
    Serial.println("Setting handle_input on server");
    // this->add_peer(this->config->client_add);
    // TODO connect to itself remove
    this->add_peer(this->config->client_add);
    // add callbacks
    esp_now_register_send_cb(this->send_input);
    esp_now_register_recv_cb(this->handle_input);
  } else {
    Serial.println("Setting send_input on client");
    this->add_peer(this->config->serv_add);
    // add callbacks
    esp_now_register_send_cb(this->send_input);
    esp_now_register_recv_cb(this->handle_input);
  }
  Mesh::buffer.fill({});
};

void Mesh::init_esp_now() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  };
  Serial.println("ESP-now initialized");
  return;
}

void Mesh::sleep() { end(); }

void Mesh::end() {
  esp_now_deinit();
  WiFi.mode(WIFI_OFF);
  printf("Deinit esp_now\n");
}

void Mesh::wakeup() { begin(); }

void Mesh::handle_input(const unsigned char *addr, const uint8_t *data,
                        int len) {

  // Serial.printf("Received: %d\n", len);

  // empty buffer
  // Mesh::buffer = {};
  // copy received bits into buffer
  memcpy(&(Mesh::buffer), data, len);

  for (keyswitch_t it : Mesh::buffer) {
    // print only non-empty
    if (it.pressed_down) {
      Serial.print(it.source);
      Serial.print(" ");
      Serial.print(it.sinc);
      Serial.print(" ");
      Serial.println(it.time);
    }
  }
};

buffer_t Mesh::buffer = {};
// KeyData Mesh::buffer = {};
// KeyData *Mesh::getBuffer() { return &Mesh::buffer; }
//
// std::vector<keyswitch_t> Mesh::buffer = {};
// std::vector<keyswitch_t> *Mesh::getBuffer() { return &Mesh::buffer; }

void Mesh::send_input(const uint8_t *addr, esp_now_send_status_t status) {

  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success"
                                                : "Delivery Fail");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Mesh::buffer = {};
  } else {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println("Delivery failed");
  }
}

void Mesh::send(std::vector<keyswitch_t> &data) {
  // this->mesh->buffer.clear();
  for (int i = 0; i < data.size(); i++) {
    Serial.println("Adding keys");
    Mesh::buffer[i % (Mesh::buffer.size() - 1)] = data[i];
    // this->mesh->buffer.push_back(this->matrix->active_keys[i]);
    if (i > (Mesh::buffer.size() - 1)) {
      this->send();
    }
    if (i == data.size() - 1) {
      this->send();
    }
  }
  // this->mesh->send();
}

void Mesh::send() {
  Serial.println(printf("Size of data %d \n", sizeof(Mesh::buffer)));
  Serial.println(Mesh::buffer.size() * sizeof(Mesh::buffer[0]));
  // Serial.println(printf("Size of keys %d\n",
  // sizeof(Mesh::buffer.active_keys)));
  esp_err_t msg = esp_now_send(this->peer.peer_addr, (uint8_t *)&Mesh::buffer,
                               sizeof(Mesh::buffer));

  if (msg == ESP_OK) {
    Serial.println("\rMsg sent:\t sucess");
  } else {
    Serial.println("\rMsg sent:\t failed");
  }
  Mesh::buffer.fill({});
}

std::vector<keyswitch_t> Mesh::get_buffer() {
  /**
   * @brief     Returns non-empty mesh keys
   */
  // std::vector<keyswitch_t> buffer = Mesh::buffer;
  std::vector<keyswitch_t> buffer;

  // buffer is an array of 6 and can be empty
  for (auto &elem : Mesh::buffer) {
    if (elem.time)
      buffer.push_back(elem);
  }

  Mesh::buffer.fill({});
  return buffer;
}

// void AdvertisedClientCallback::onResult(BLEAdvertisedDevice *other) {
//   if (other->isAdvertisingService(BLEUUID(split_channel_service_uuid))) {
//     printf("Found other service!\n");
//     printf("Found device %s \n", other->toString().c_str());
//     BLEDevice::getScan()->stop();
//     // scanning = false;
//     host_dev = other;
//   }
// }
