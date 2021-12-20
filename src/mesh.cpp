#include "mesh.hpp"

#include <freertos/semphr.h>
/** Design
 *  The keyboard consists of n-parts where only the server knows
 * what each parts should encode. In this way keybindings can be encoded
 *
 **/

const char *split_channel_service_uuid = "ee583eec-576b-11ec-bf63-0242ac130002";
const char *split_message_uuid = "ee58419e-576b-11ec-bf63-0242ac130002";
static SemaphoreHandle_t mesh_mutex = xSemaphoreCreateMutex();
void handle_input(const unsigned char *addr, const uint8_t *data, int len){};

void scan_ended_cb(NimBLEScanResults results) { printf("Scan Ended\n"); }
Mesh::Mesh(Config *config) {
  Serial.println("Setting up mesh connection");
  this->config = config;

  // not the keyboard, warning: ble keyboard needs to be initialized
  // first
  // TODO: add flag for role in mesh
  if (!BLEDevice::getInitialized()) {
    BLEDevice::init(config->device_name);
    // server if no ble is activated yet
    // keyboard will activate ble
    is_server = true;
  }

  // initialize the channel service and characteristic
  BLEServer *server = BLEDevice::createServer();
  BLEService *channel_service =
      server->createService(split_channel_service_uuid);
  BLECharacteristic *message_characteristic =
      channel_service->createCharacteristic(split_message_uuid,
                                            NIMBLE_PROPERTY::READ_ENC |
                                                NIMBLE_PROPERTY::WRITE_ENC |
                                                NIMBLE_PROPERTY::NOTIFY);
  message_characteristic->setCallbacks(this);
  if (is_server) {
    // only start service if it is a server
    if (channel_service->start()) {
      printf("Channel service for mesh  not started\n");
    }
    printf("Channel service started\n");
  } else {
    // start scanning for a client
    uint8_t scan_interval = 25;
    uint8_t scan_window = scan_interval - 1;
    uint8_t scan_time = 1;

    // start scanning
    BLEScan *scan = BLEDevice::getScan();
    scan->setAdvertisedDeviceCallbacks(this);
    scan->setActiveScan(true);
    scan->setInterval(scan_interval);
    scan->setWindow(scan_window);
    scan->start(scan_time, &scan_ended_cb);
  }

  Mesh::buffer.clear(); // redundant
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
  Mesh::buffer.clear();
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
      printf("Mesh key %d %d T: %d\n", it.source, it.sinc, it.time);
    }
  }
};

void Mesh::send_input(const uint8_t *addr, esp_now_send_status_t status) {

  printf("\r\nLast Packet Send Status:\t");
  printf("status == %s \n",
         ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

  if (status == ESP_NOW_SEND_SUCCESS) {
    Mesh::buffer = {};
  } else {
    printf("\r\nLast Packet Send Status:\t Delivery failed\n");
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
  Mesh::buffer.clear();
}

std::vector<keyswitch_t> Mesh::get_buffer() {
  /**
   * @brief     Returns non-empty mesh keys
   **/
  // std::vector<keyswitch_t> buffer = Mesh::buffer;
  std::vector<keyswitch_t> buffer;

  // buffer is an array of 6 and can be empty
  if (xSemaphoreTake(mesh_mutex, 0) == pdTRUE) {

    for (auto &elem : Mesh::buffer) {
      if (elem.time)
        buffer.push_back(elem);
    }
    Mesh::buffer.clear();
    xSemaphoreGive(mesh_mutex);
  }
  return buffer;
}

// mesh client
void Mesh::onDisconnect(BLEClient *client) {
  uint8_t scan_interval = 25;
  uint8_t scan_window = scan_interval - 1;
  uint8_t scan_time = 1;

  // start scanning
  BLEScan *scan = BLEDevice::getScan();
  scan->setActiveScan(true);
  scan->setInterval(scan_interval);
  scan->setWindow(scan_window);
  scan->start(scan_time, &scan_ended_cb);
}

BLEClient *lookup_client(BLEAdvertisedDevice *host_dev) {
  /**
   * @brief      Looks up client on host device
   *
   * @param      BLEAdvertisedDevice host_dev: host device from  scan result
   *
   * @return     BLEClient* (possibly null)
   **/

  BLEClient *client = nullptr;

  // check existing connections
  if (host_dev) {
    // host device was found at some time
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
  }
  // no host device found
  else {
    printf("No host device found!\n");
  }
  return client;
}

// TODO tie this to a specific host_dev?
// For a dynamic mesh multiple clients will be made
BLEClient *Mesh::create_client(BLEAdvertisedDevice *host_dev) {
  /**
   * @details    Setup client for mesh communication
   * It performs
   * 1. check if client already exists
   * 2. sets up the connection params
   *
   * @return     return type
   */
  // looking up client
  BLEClient *client = lookup_client(host_dev);
  if (!client) {
    // create new client
    if (BLEDevice::getClientListSize() >= BLE_MAX_CONNECTIONS) {
      printf("Max client size reached - no more connections available\n");
    } else {
      printf("Creating client\n");
      client = BLEDevice::createClient(host_dev->getAddress());
      printf("New client created\n");

      printf("Setting connection parameters\n");
      client->setClientCallbacks(this, false);
      client->setConnectionParams(12, 12, 0, 51);
      client->setConnectTimeout(5);
    }
  }
  return client;
}

/**
 * @brief Handle events from mesh network
 *
 * @details Copy  the information from the  mesh server into
 * the main keyboard loop
 */
void Mesh::notify_cb(BLERemoteCharacteristic *remoteCharacteristic,
                     uint8_t *data, size_t length, bool isNotify) {

  // TODO: check whether this does not cause infinite wait time with racing
  // condition
  printf("Received message from %s\n", remoteCharacteristic->getUUID());
  message_t msg;
  // wait for mutex release
  printf("Waiting for release of mesh mutex\n");
  while (!(xSemaphoreTake(mesh_mutex, 0) == pdTRUE)) {
  }
  printf("Mesh mutex obtained\n");
  memcpy(&msg, (message_t *)data, length);
  for (keyswitch_t &elem : msg) {
    // only keep initialized keys
    if (elem.time)
      Mesh::buffer.push_back(elem);
  }
  printf("Added %d keys to mesh buffer\n", length);
  xSemaphoreGive(mesh_mutex);
}

bool Mesh::connect(BLEClient *client) {
  /**
   * @brief     Connect to the host
   * @return    true if correct, false otherwise
   */

  // ensure client connection exists
  if (client == nullptr) {
    printf("Client not initialized!\n");
    return false;
  }

  // attempt connect
  if (!client->isConnected()) {
    if (!client->connect(false)) {
      printf("Failed to connect \n");
      return false;
    }
  }

  // ensure secure connection
  if (client->secureConnection()) {
    printf("Secure connection achieved\n");
  } else {
    printf("Warning connection is not secure\n");
  }

  printf("Connected to: %s\n", client->getPeerAddress().toString().c_str());
  printf("SSRI: %d \n", client->getRssi());

  // setup connection & subscribe to remote characteristic
  BLERemoteService *remote_service;
  BLERemoteCharacteristic *remote_characteristic;
  // BLERemoteDescriptor *remote_description;

  // subscribe to characterstic
  remote_service = client->getService(split_channel_service_uuid);
  if (remote_service) {
    // subscribe to server for future notification
    remote_characteristic =
        remote_service->getCharacteristic(split_message_uuid);
    // subscribe to remote characteristic
    if (remote_characteristic->canNotify()) {
      if (!remote_characteristic->subscribe(true, Mesh::notify_cb)) {
        client->disconnect();
        return false;
      }
      printf("Subscribed to remote characteristic\n");
    }
  } else {
    printf("Service not found\n");
    return false;
  }
  printf("Done with device!\n");
  return true;
}

// end mesh client

// mesh advertising
void Mesh::onResult(BLEAdvertisedDevice *other) {
  if (other->isAdvertisingService(BLEUUID(split_channel_service_uuid))) {
    printf("Found other service!\n");
    printf("Found device %s \n", other->toString().c_str());
    // TODO: push this into the mesh?
    connect(create_client(other));
  }
}
// void scan_start() {
//   uint8_t scan_interval = 25;
//   uint8_t scan_window = scan_interval - 1;
//   uint8_t scan_time = 1;

//   // start scanning
//   BLEScan *scan = BLEDevice::getScan();
//   // only 1 exists, overwrites if present
//   AdvertisedClientCallback advertised_cb = AdvertisedClientCallback();
//   scan->setAdvertisedDeviceCallbacks(&advertised_cb);
//   scan->setActiveScan(true);
//   scan->setInterval(scan_interval);
//   scan->setWindow(scan_window);
//   scan->start(scan_time, &scan_ended_cb);
// }
// end advertising
