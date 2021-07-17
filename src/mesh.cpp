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

  // Serial.print("Received ");
  // Serial.println(len);

  // empty buffer
  // Mesh::buffer = {};
  // copy received bits into buffer
  memcpy(&(Mesh::buffer), data, len);

  // for (keyswitch_t it : Mesh::buffer) {
  //   Serial.print(it.source);
  //   Serial.print(" ");
  //   Serial.print(it.sinc);
  //   Serial.print(" ");
  //   Serial.println(it.time);
  // }
};

buffer_t Mesh::buffer = {};
// KeyData Mesh::buffer = {};
// KeyData *Mesh::getBuffer() { return &Mesh::buffer; }
//
// std::vector<keyswitch_t> Mesh::buffer = {};
// std::vector<keyswitch_t> *Mesh::getBuffer() { return &Mesh::buffer; }

void Mesh::send_input(const uint8_t *addr, esp_now_send_status_t status) {

  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success"
  // : "Delivery Fail");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Mesh::buffer = {};
  } else {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println("Delivery failed");
  }
}

void Mesh::send(std::vector<keyswitch_t> data) {
  // esp_now_send(reinterpret_cast<const uint8_t
  // *>(this->config->server_address), (uint8_t *)&data, sizeof(data));

  Serial.println(printf("\rSize of data %d \n", sizeof(data)));
  Serial.println(sizeof(data[0]) * data.size());
  // Serial.println(printf("Size of keys %d\n", sizeof(data.active_keys)));

  esp_err_t msg = esp_now_send(this->peer.peer_addr, (uint8_t *)&data,
                               sizeof(data[0]) * data.size());

  if (msg == ESP_OK) {
    Serial.print("\rMsg sent:\t sucess\n");
  } else {
    Serial.print("\rMsg sent:\t failed\n");
  }
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
}

buffer_t Mesh::get_buffer() {
  auto buffer = Mesh::buffer;
  Mesh::buffer.fill({});
  return buffer;
}
