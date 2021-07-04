#include "mesh.hpp"

// needs functions on data received for server
// and for client; in this case let's focus only on the
// server

// logs if a message was sent correctly

// TODO
void handle_input(const unsigned char *addr, const uint8_t *data, int len){};

Mesh::Mesh() {
  WiFi.mode(WIFI_MODE_STA);
  // initialize wifi before esp-now
  this->init_esp_now();
}

Mesh::Mesh(Config *config) : Mesh() {
  this->config = config;
  if (config->server_address == WiFi.macAddress().c_str()) {
    // init client stuff
    Serial.println("Setting handle_input on server");
    esp_now_register_recv_cb(this->handle_input);
  } else {
    Serial.println("Setting send_input on client");
    esp_now_register_send_cb(this->send_input);
  }
}

void Mesh::add_peer(const uint8_t *peer_addr) {
  esp_now_peer_info_t peer;
  memcpy(peer.peer_addr, peer_addr, 6);
  peer.channel = 0;
  peer.encrypt = false;

  if (esp_now_add_peer(&peer) == ESP_OK) {
    Serial.println("Failed to add peer");
  } else {
    Serial.println("Added peer");
  }
}

void Mesh::init_esp_now() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  };
  Serial.println("ESP-now initialized");
  return;
}

void Mesh::handle_input(const unsigned char *addr, const uint8_t *data,
                        int len) {
  // empty buffer
  Mesh::buffer = {};
  // copy received bits into buffer
  memcpy(&(Mesh::buffer), data, len);
};

KeyData Mesh::buffer = {};
KeyData *Mesh::getBuffer() { return &Mesh::buffer; }

void Mesh::send_input(const unsigned char *addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success"
                                                : "Delivery Fail");
}

void Mesh::send(KeyData data) {
  esp_now_send(reinterpret_cast<const uint8_t *>(this->config->server_address),
               (uint8_t *)&data, sizeof(data));
}
