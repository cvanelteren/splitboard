#include "now.hpp"

// needs functions on data received for server
// and for client; in this case let's focus only on the
// server

// logs if a message was sent correctly
void send_input(const unsigned char *addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success"
                                                : "Delivery Fail");
}

// TODO
void handle_input(const unsigned char *addr, const uint8_t *data, int len){};

EspNow::EspNow() {
  WiFi.mode(WIFI_MODE_STA);
  // initialize wifi before esp-now
  this->init_esp_now();
}

EspNow::EspNow(Config *config) : EspNow() {
  if (config->server_address != WiFi.macAddress().c_str()) {
    // init client stuff
    esp_now_register_send_cb(send_input);
  } else {
    esp_now_register_recv_cb(handle_input);
  }
}

void EspNow::add_peer(const uint8_t *peer_addr) {
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

void EspNow::init_esp_now() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  };
  return;
}
