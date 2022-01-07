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
// I am expecting that this feature does not improve the battery life. I
// would have to test the current draw on both halves to determine if there is
// any significant difference in current draw.
// TODO: test dynamic mesh, i.e. multiple clients presented to the HID.
// TODO: perform password pairing between halves, show passwords on the display.
const char *split_channel_service_uuid = "ee583eec-576b-11ec-bf63-0242ac130002";
const char *split_message_uuid = "ee58419e-576b-11ec-bf63-0242ac130002";
const char *split_event_uuid = "2cbd927c-67ed-11ec-90d6-0242ac120003";

static SemaphoreHandle_t mesh_mutex = xSemaphoreCreateMutex();
static SemaphoreHandle_t mesh_event_mutex = xSemaphoreCreateMutex();

NimBLEAdvertisedDevice *HOST;

std::vector<keyswitch_t> Mesh::buffer = {};

void scan_ended_cb(BLEScanResults results) { printf("Scan Ended\n"); }

Mesh::Mesh(Config *config) {
  printf("Setting up mesh connection\n");
  this->config = config;
  has_connection = false;
}

void Mesh::begin() {
  // not the keyboard, warning: ble keyboard needs to be initialized
  // first
  // TODO: add flag for role in mesh

  if (!BLEDevice::getInitialized()) {
    BLEDevice::init("");
  }

  std::string servAddr = NimBLEAddress(config->serv_add).toString();
  printf(BLEDevice::getAddress().toString().c_str());
  printf("\n");
  printf(servAddr.c_str());
  printf("\n%d\n", BLEDevice::getAddress().toString().compare(servAddr));

  // only hub for non-servers
  if (BLEDevice::getAddress().toString().compare(servAddr) == 0) {
    is_hub = false;
  } else {
    is_hub = true;
  }

  printf("IS HUB %d\n", is_hub);
  if (is_hub) {
    BLEDevice::init(config->device_name + "_hub");
    // server if no ble is activated yet
    // keyboard will activate ble
    is_hub = true;
    // NimBLEDevice::setSecurityPasskey(123456);
    // initialize the channel service and characteristic

    BLEDevice::setSecurityAuth(true, true, true);
    BLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
    BLEServer *server = BLEDevice::createServer();

    channel_service = server->getServiceByUUID(split_channel_service_uuid);
    if (channel_service == nullptr) {
      printf("Creating secret service\n");
      channel_service = server->createService(split_channel_service_uuid);
    }

    message_characteristic =
        channel_service->getCharacteristic(split_message_uuid);

    if (message_characteristic == nullptr) {
      printf("Creating message characteristic \n");
      message_characteristic = channel_service->createCharacteristic(
          split_message_uuid, NIMBLE_PROPERTY::NOTIFY);
    }

    message_characteristic->setCallbacks(this);

    event_characteristic = channel_service->getCharacteristic(split_event_uuid);
    if (event_characteristic == nullptr) {
      printf("Creating event characteristic\n");
      event_characteristic = channel_service->createCharacteristic(
          split_event_uuid, NIMBLE_PROPERTY::NOTIFY);
    }
    event_characteristic->setCallbacks(this);

    channel_service->start();
    printf("Channel service started\n");

    server->advertiseOnDisconnect(true);
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setScanResponse(false);
    pAdvertising->addServiceUUID(split_channel_service_uuid);
    pAdvertising->setMinPreferred(0x06);
    printf("Starting to advertise\n");
    pAdvertising->start();
  }

  Mesh::buffer.clear();
}

void Mesh::scan() {
  printf("Starting scan\n");

  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  uint8_t scan_interval = 100;
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

// void Mesh::send(const std::vector<keyswitch_t> &data,
//                 BLECharacteristic *characteristic) {
//   if (data.size()) {
//     characteristic->setValue((uint8_t *)&data, sizeof(data[0]) *
//     data.size()); characteristic->notify(true);
//   }
// }
// void Mesh::send(const std::vector<event_t> &data,
//                 BLECharacteristic *characteristic) {
//   if (data.size()) {
//     characteristic->setValue((uint8_t *)&data, sizeof(data[0]) *
//     data.size()); characteristic->notify(true);
//   }
// }

std::vector<keyswitch_t> Mesh::get_buffer() {
  /**
   * @brief     Returns non-empty mesh keys
   **/
  // std::vector<keyswitch_t> buffer = Mesh::buffer;
  std::vector<keyswitch_t> buffer;

  // buffer is an array of 6 and can be empty
  if (xSemaphoreTake(mesh_mutex, 0) == pdTRUE) {
    for (auto &elem : Mesh::buffer) {
      if (elem.time) {
        buffer.push_back(elem);
      }
    }
    Mesh::buffer.clear();
    xSemaphoreGive(mesh_mutex);
  }
  return buffer;
}

// mesh client
void Mesh::onDisconnect(BLEClient *client) {
  printf("Client disconnected\n");
  has_connection = false;
}

#include "event_manager.hpp"
// bool Mesh::onConfirmPIN(uint32_t pass_key) {
//   // TODO: need some other way to interface with the event manager;
//   extern EventManager manager;
//   // confirm pin from the server
//   const uint32_t timeout = 500000; // milliseconds
//   size_t start = millis();
//   printf("Received password %d \n", pass_key);
//   // manager.add_event(display_event(pass_key));
//   while (((millis() - start) < timeout)) {
//     // read internal button
//     if (digitalRead(GPIO_NUM_0)) {
//       printf("Pin confirmed");
//       return true;
//     }
//   }
//   printf(("Pin timeout elapsed, connection refused\n"));
//   return false;
// };

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
  if (host_dev != nullptr) {
    // host device was found at some time
    printf("Looking for existing client\n");
    // check for existing clients
    if (BLEDevice::getClientListSize()) {
      printf("Client size list %d\n", BLEDevice::getClientListSize());
      client = BLEDevice::getClientByPeerAddress(host_dev->getAddress());
      if (client) {
        printf("Found client in list\n");
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

  // // looking up client
  BLEClient *client = lookup_client(host_dev);
  if (!client) {
    // create new client
    if (BLEDevice::getClientListSize() >= BLE_MAX_CONNECTIONS) {
      printf("Max client size reached - no more connections available\n");
      BLEDevice::getScan()->stop();
    } else {
      printf("Creating client\n");
      printf("TYPE IS %d\n", host_dev->getAddress().getType());
      if (host_dev) {
        client = BLEDevice::createClient(host_dev->getAddress());
        // client = BLEDevice::createClient();
        printf("New client created\n");

        printf("Setting connection parameters\n");
        client->setClientCallbacks(this, false);
        client->setConnectionParams(12, 12, 0, 51);
        client->setConnectTimeout(5);
        // if (!client->connect(host_dev, false)) {
        //   BLEDevice::deleteClient(client);
        //   printf("Failed to connect, deleted client\n");
        //   client = nullptr;
        // }

      } else {
        printf("host_dev does not exist\n");
      }
    }
  }
  return client;
}

bool Mesh::connectServer() {
  NimBLEClient *client = nullptr;

  printf("Attempting connection");
  if (host_dev == nullptr) {
    printf("no host dev found\n");
    return false;
  }
  // check for existing clients
  if (BLEDevice::getClientListSize()) {
    printf("Client size list %d\n", BLEDevice::getClientListSize());
    client = BLEDevice::getClientByPeerAddress(host_dev->getAddress());
    if (client) {
      if (!(client->connect(host_dev, false))) {
        printf("Reconnect failed\n");
        return false;
      }
      printf("Reconnected to client");
    } else {
      client = BLEDevice::getDisconnectedClient();
    }
  }

  if (!client) {
    if (BLEDevice::getClientListSize() >= NIMBLE_MAX_CONNECTIONS) {
      printf("Max client size reached - no more connections available\n");
      return false;
    }

    printf("Creating client\n");
    client = BLEDevice::createClient(host_dev->getAddress());
    printf("New client created\n");

    printf("Setting connection parameters\n");
    client->setClientCallbacks(this, false);
    client->setConnectionParams(12, 12, 0, 51);
    client->setConnectTimeout(5);
    printf("Params set\n");

    if (!client->connect()) {
      BLEDevice::deleteClient(client);
      printf("Failed to connect, deleted client\n");
      return false;
    }
  }

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

  printf("Testing here");

  printf("Connected to: %s\n", client->getPeerAddress().toString().c_str());
  printf("SSRI: %d \n", client->getRssi());

  NimBLERemoteService *remote_service;
  NimBLERemoteCharacteristic *remote_characteristic;
  NimBLERemoteDescriptor *remote_description;

  remote_service = client->getService(split_channel_service_uuid);
  if (remote_service) {
    remote_characteristic =
        remote_service->getCharacteristic(split_message_uuid);
    if (remote_characteristic && remote_characteristic->canRead()) {
    }

    if (remote_characteristic->canNotify()) {
      // if(!pChr->registerForNotify(notifyCB)) {
      if (!remote_characteristic->subscribe(true, &Mesh::retrieve_keys)) {
        client->disconnect();
        return false;
      }
    }

  } else {
    printf("Service not found\n");
  }

  printf("Done with device!\n");
  return true;
}

/**
 * @brief Handle events from mesh network
 *
 * @details Copy  the information from the  mesh server into
 * the main keyboard loop
 */
void Mesh::retrieve_keys(BLERemoteCharacteristic *remoteCharacteristic,
                         uint8_t *data, size_t length, bool isNotify) {

  // TODO: check whether this does not cause infinite wait time with racing
  // condition
  printf("Received message from %s\n",
         remoteCharacteristic->getUUID().toString().c_str());
  // wait for mutex release
  printf("Waiting for release of mesh mutex\n");
  while (!(xSemaphoreTake(mesh_mutex, 0) == pdTRUE)) {
    taskYIELD();
  }
  printf("Mesh mutex obtained\n");

  std::vector<keyswitch_t> msg;
  // printf("Resized %d \n", length);
  msg.resize(length / sizeof(keyswitch_t));
  memcpy(&msg[0], data, length);

  // check for valid keys
  for (keyswitch_t &elem : msg) {
    // only keep initialized keys
    if (elem.time)
      Mesh::buffer.push_back(elem);
  }
  printf("Added %d keys to mesh buffer\n", msg.size());
  xSemaphoreGive(mesh_mutex);
}

#include "event_manager.hpp"
#include "keyboard.hpp"
#include "keyboard_events.hpp"

extern Keyboard keyboard;
extern bool (*KEYBOARD_EVENTS[])();
void Mesh::retrieve_events(BLERemoteCharacteristic *remoteCharacteristic,
                           uint8_t *data, size_t length, bool isNotify) {

  /**
   * @brief      Channel that deals with "special events that are not normal
   * keys.
   */

  // TODO: check whether this does not cause infinite wait time with racing
  // condition
  printf("Received message from %s\n",
         remoteCharacteristic->getUUID().toString().c_str());
  // wait for mutex release
  printf("Waiting for release of mesh event mutex\n");
  while (!(xSemaphoreTake(mesh_event_mutex, 0) == pdTRUE)) {
    taskYIELD();
  }

  printf("Mesh event mutex obtained\n");

  std::vector<event_t> events;
  events.resize(length / sizeof(event_t));
  memcpy(&events[0], data, length);
  for (auto &event : events) {
    try {
      keyboard.manager->add_event(KEYBOARD_EVENTS[event]);
    } catch (const std::exception &e) {
      printf("Adding keyboard event failed\n");
    }
  }

  xSemaphoreGive(mesh_event_mutex);
}

void Mesh::update() {
  static size_t last_time;
  if ((is_hub == false))
    if (is_connected() == false) {
      if (!connectServer()) {
        scan();
      }
    }
}

void Mesh::onConnect(BLEClient *client) {
  /** Default connnection  parameters are min 20  ms and max
   *40ms with a timeout of  4 seconds. In the tutorial there
   *is a slower  connection shown. * below  is min interval,
   *max interval, latency, timeout. The values are multiples of 1.25ms except
   *the timeout which is *10ms.
   **/

  printf("Connected\n");
  printf("Connected to client \n");
  client->updateConnParams(0, 5, 0, 60);
  has_connection = true;
}

bool subscribe_to(std::string characteristic_uuid,
                  BLERemoteService *remote_service, BLEClient *client) {
  BLERemoteCharacteristic *remote_characteristic;

  // subscribe to characterstic
  void (*cb)(BLERemoteCharacteristic * remoteCharacteristic, uint8_t * data,
             size_t length, bool isNotify);

  if (characteristic_uuid == split_message_uuid) {
    printf("Trying split_message\n");
    cb = &Mesh::retrieve_keys;
  } else if (characteristic_uuid == split_event_uuid) {
    printf("Trying events\n");
    cb = &Mesh::retrieve_events;
  } else {
    printf("No valid characteristic uuid given for subscription\n");
    return true;
  }

  if (remote_service) {
    // subscribe to server for future notification
    remote_characteristic =
        remote_service->getCharacteristic(characteristic_uuid);
    // subscribe to remote characteristic
    if (remote_characteristic->canNotify()) {
      printf("REMOTE SERVICE CAN NOTIFY\n");
      if (!remote_characteristic->subscribe(true, *cb)) {
        printf("Cannot subscribe to characteristic \n");
        client->disconnect();
        return true;
      }
      printf("Subscribed to remote characteristic\n");
    }
  } else {
    printf("Service not found\n");
    return true;
  }
  return false;
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
    printf("Attempting to connect\n");
    if (!client->connect(false)) {
      printf("Failed to connect \n");
      BLEDevice::deleteClient(client);
      return false;
    }
  }

  printf("Connection achieved\n");
  // ensure secure connection
  if (client->secureConnection()) {
    printf("Secure connection achieved\n");
  } else {
    printf("Warning connection is not secure\n");
  }

  printf("Connected to: %s\n", client->getPeerAddress().toString().c_str());
  printf("SSRI: %d \n", client->getRssi());
  BLERemoteService *remote_service =
      client->getService(split_channel_service_uuid);
  if (remote_service) {
    printf("Found remote service\n");
    if (subscribe_to(split_channel_service_uuid, remote_service, client)) {
      printf("Failed to subscribe\n");
    }
  }
  printf("Done with device!\n");
  // has_connection = true;
  return true;
}

// mesh advertising
void Mesh::onResult(BLEAdvertisedDevice *other) {
  // printf("Found device %s \n", other->toString().c_str());
  if (other->isAdvertisingService(BLEUUID(split_channel_service_uuid))) {
    printf("Found other service!\n");
    printf("Found device %s \n", other->toString().c_str());
    host_dev = other;
  }
}

// template Mesh::send(const std::vector<keyswitch_t> &data,
//                     BLECharacteristic *characteristic);
// template Mesh::send(const std::vector<event_t> &data,
//                     BLECharacteristic *characteristic);

bool Mesh::is_connected() { return has_connection; }
