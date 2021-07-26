#include "mesh.hpp"

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

  // for (auto i = 0; i < 6; i++) {
  //   Serial.print(this->peer.peer_addr[i]);
  //   Serial.print(" ");
  // }
  // Serial.println();

  // this->peer.ifidx = WIFI_IF_AP;

  // Serial.println(printf("%s", this->peer.peer_addr));
  // Serial.println(printf("%d", this->peer.channel));

  if (esp_now_add_peer(&(this->peer)) != ESP_OK) {
    Serial.println("Failed to add peer");
  } else {
    Serial.println("Added peer");
  }
}

void Mesh::begin() {
  this->init_esp_now();
  // add peer
  if (WiFi.macAddress() == this->config->server_address) {
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

void Mesh::handle_input(const unsigned char *addr, const uint8_t *data,
                        int len) {

  // Serial.printf("Received: %d\n", len);

  // empty buffer
  // Mesh::buffer = {};
  // copy received bits into buffer
  memcpy(&(Mesh::buffer), data, len);

  for (keyswitch_t it : Mesh::buffer) {
    // print only non-empty
    if (it.active) {
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
  std::vector<keyswitch_t> buffer;
  for (auto &elem : Mesh::buffer) {
    if (elem.time)
      buffer.push_back(elem);
  }
  Mesh::buffer.fill({});
  return buffer;
}
