#include "mesh.hpp"
#include <freertos/semphr.h>
/** Design
 *  The  keyboard  consists of  n-parts.  Each  part
 *  encodes a  particular hard coded role  it fullfills. Who
 *  is the server and who is the client may change, but they
 *  role they fullfill should be the same.
 *
 *  Each unit  can function as  a server  or as a  client. A
 *  server hosts the keys that  are being pressed by a user,
 *  a client  connects to the  servers and pushes it  to the
 *  the external client.  That is, the HID acts  as a server
 *  which the  client (e.g.  phone, computer  etc), connects
 *  to.  The  HID  server  also  has  several  clients  that
 *  connects to other servers in  the mesh. Each of the mesh
 *  servers present their pressed keys.
 *
 *  At the  time of wrinting,  there is one mesh  client and
 *  one mesh server.
 **/

// TODO: Implement dynamic client/server role in system for battery balancing.
// TODO: test dynamic mesh, i.e. multiple clients presented to the HID.
const char *split_channel_service_uuid = "ee583eec-576b-11ec-bf63-0242ac130002";
const char *split_message_uuid = "ee58419e-576b-11ec-bf63-0242ac130002";
static SemaphoreHandle_t mesh_mutex = xSemaphoreCreateMutex();

void scan_ended_cb(NimBLEScanResults results) { printf("Scan Ended\n"); }
Mesh::Mesh(Config *config) {
  Serial.println("Setting up mesh connection");
  this->config = config;
}

void Mesh::begin() {
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
  channel_service = server->createService(split_channel_service_uuid);
  message_characteristic = channel_service->createCharacteristic(
      split_message_uuid, NIMBLE_PROPERTY::READ_ENC |
                              NIMBLE_PROPERTY::WRITE_ENC |
                              NIMBLE_PROPERTY::NOTIFY);
  message_characteristic->setCallbacks(this);
  message_characteristic->notify(true);
  if (is_server) {
    // only start service if it is a server
    if (channel_service->start()) {
      printf("Channel service for mesh  not started\n");
    }
    printf("Channel service started\n");
  } else {
    // start scanning for a client
    scan();
  }
  Mesh::buffer.clear(); // redundant
}

void Mesh::scan() {
  printf("Starting scan\n");
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

void Mesh::sleep() { end(); }

void Mesh::end() { BLEDevice::deinit(false); }

void Mesh::wakeup() { begin(); }

void Mesh::send(std::vector<keyswitch_t> &data) {
  // fill the characterstic & notify subscribed clients
  if (data.size()) {
    message_characteristic->setValue((uint8_t *)&data, sizeof(data));
    message_characteristic->notify(true);
  }
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
  printf("Client disconnected\n");
  scan();
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

bool Mesh::connect_to_server(BLEClient *client) {
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

// mesh advertising
void Mesh::onResult(BLEAdvertisedDevice *other) {
  if (other->isAdvertisingService(BLEUUID(split_channel_service_uuid))) {
    printf("Found other service!\n");
    printf("Found device %s \n", other->toString().c_str());
    if (!is_server) {
      connect_to_server(create_client(other));
    }
    // look for other devices in mesh
    if (BLEDevice::getClientListSize() < COMPONENTS_IN_MESH) {
      scan();
    } else {
      BLEDevice::getScan()->stop();
    }
  }
}
