#include "mesh.hpp"

// TODO: write a keycode msg buffer
// remove the static class property
// add return types to the send functions
// needs functions on data received for server
// and for client; in this case let's focus only on the
// server

// logs if a message was sent correctly
// TODO
//

const char *split_channel_service_uuid = "ee583eec-576b-11ec-bf63-0242ac130002";
const char *split_message_uuid = "ee58419e-576b-11ec-bf63-0242ac130002";

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

void scan_ended_cb(NimBLEScanResults results) { printf("Scan Ended\n"); }

MeshClient::MeshClient(Config *confg) {

  // if (!BLEDevice::getInitialized()) {
  //   BLEDevice::init("Something went wrong");
  // }

  // setup callback
  advertised_cb = AdvertisedClientCallback();
  client_cb = ClientCallback();

  create_client();
  BLEScan *scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(&advertised_cb);
  scan->setActiveScan(true);
  scan->setInterval(40);
  scan->setWindow(39);
  size_t scanTime = 1;
  scan->start(scanTime, &scan_ended_cb);
}

BLEClient *lookup_client(BLEAdvertisedDevice *host_dev) {
  /**
   * @brief      Looks up client on host device
   *
   * @param      BLEAdvertisedDevice host_dev: host device from  scan result
   *
   * @return     BLEClient* (possible null)
   **/

  BLEClient *client = nullptr;
  printf("Looking for existing client\n");
  // check for existing clients
  if (BLEDevice::getClientListSize()) {
    printf("Client size list %d\n", BLEDevice::getClientListSize());
    client = BLEDevice::getClientByPeerAddress(host_dev->getAddress());
    if (client) {
      if (!(client->connect(host_dev, false))) {
        printf("Reconnect failed\n");
        return client;
      }
      printf("Reconnected to client");
    } else {
      client = BLEDevice::getDisconnectedClient();
    }
  }
  return client;
}

void MeshClient::create_client() {
  /**
   * @details    Setup client for mesh communication
   * It performs
   * 1. check if client already exists
   * 2. sets up the connection params
   *
   * @return     return type
   */
  // looking up client
  client = lookup_client(&advertised_cb.host_dev);
  if (!client) {
    // create new client
    if (BLEDevice::getClientListSize() >= BLE_MAX_CONNECTIONS) {
      printf("Max client size reached - no more connections available\n");
    }

    printf("Creating client\n");
    client = BLEDevice::createClient();
    printf("New client created\n");

    printf("Setting connection parameters\n");
    client->setClientCallbacks(&client_cb, false);
    client->setConnectionParams(12, 12, 0, 51);
    client->setConnectTimeout(5);
  }
}

// controls the write on receive of message
void MeshClient::notify_cb(BLERemoteCharacteristic *remoteCharacteristic,
                           uint8_t *data, size_t length, bool isNotify) {

  // make some stuff happen here
}
bool MeshClient::connect() {
  /**
   * @brief     Connect to the host
   * @return    true if correct, false otherwise
   */

  if (client == nullptr) {
    printf("Client not initialized!\n");
    return false;
  }

  if (!client->isConnected()) {
    if (!client->connect(&advertised_cb.host_dev, false)) {
      printf("Failed to connect \n");
      return false;
    }
  }
  // debug
  printf("Connected to: %s\n", client->getPeerAddress().toString().c_str());
  printf("SSRI: %d \n", client->getRssi());

  // setup connection
  BLERemoteService *remote_service;
  BLERemoteCharacteristic *remote_characteristic;
  // BLERemoteDescriptor *remote_description;

  // subscribe to characterstic
  remote_service = client->getService(split_channel_service_uuid);
  if (remote_service) {
    // subscribe to server for future notification
    remote_characteristic =
        remote_service->getCharacteristic(split_message_uuid);
    if (remote_characteristic->canNotify()) {
      if (!remote_characteristic->subscribe(true, notify_cb)) {
        client->disconnect();
        return false;
      }
    }

  } else {
    printf("Service not found\n");
    return false;
  }

  printf("Done with device!\n");
  return true;
}

MeshServer::MeshServer(Config *Config) {
  // if (!BLEDevice::getInitialized()) {
  // BLEDevice::init();
  // }

  // NimBLEDevice::setSecurityAuth(true, true, true);
  // NimBLEDevice::setSecurityPasskey(123456);
  // NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);

  // create server
  server = BLEDevice::createServer();

  // setup callback
  server_cb = new ServerCallback();
  server->setCallbacks(server_cb);

  // configure service and make characteristic available
  channel_service = server->createService(split_channel_service_uuid);
  message_characteristic = channel_service->createCharacteristic(
      split_message_uuid,
      NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);
  message_characteristic->setCallbacks(&client_cb);

  // advertising settings
  server->advertiseOnDisconnect(true);

  BLEAdvertising *advertising = server->getAdvertising();
  advertising->addServiceUUID(split_channel_service_uuid);
  advertising->setScanResponse(true);
  advertising->setMinPreferred(
      0x06); // functions that help with iPhone connections issue
  advertising->setMinPreferred(0x12);

  // spin up server
  channel_service->start();
  server->startAdvertising();
}
