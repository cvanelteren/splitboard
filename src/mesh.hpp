#ifndef mesh_hpp
#define mesh_hpp
#include "config.hpp"

#include "types.hpp"
#include <WiFi.h>
#include <esp_now.h>

#if defined(USE_NIMBLE)
#include "NimBLECharacteristic.h"
#include "NimBLEHIDDevice.h"
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>

#define BLEDevice NimBLEDevice
#define BLEServerCallbacks NimBLEServerCallbacks
#define BLECharacteristicCallbacks NimBLECharacteristicCallbacks
#define BLEHIDDevice NimBLEHIDDevice
#define BLECharacteristic NimBLECharacteristic
#define BLEAdvertising NimBLEAdvertising
#define BLEServer NimBLEServer
#define BLAdvertisedDeviceCallbacks NimBLEAdvertisedDeviceCallbacks

#define BLE_MAX_CONNECTIONS NIMBLE_MAX_CONNECTIONS

#else
#include "BLE2902.h"
#include "BLECharacteristic.h"
#include "BLEHIDDevice.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#endif // USE_NIMBLE

typedef struct {
  uint16_t keys;
  uint8_t layer;

} msg_t;

// Handle events for server
class ServerCallback : public BLEServerCallbacks {
public:
  ServerCallback();
  // connection
  void onConnect(BLEServer *server, ble_gap_conn_desc *desc);
  void onDisconnect(BLEServer *server);
  // security
  uint32_t onPassKeyRequest();
  bool onConfirmPIN(uint32_t pass_key);
  void onAuthenticationComplete(ble_gap_conn_desc *desc);
  // sending
  void onNotify(BLECharacteristic *characteristic);
  void onSubscribe(BLECharacteristic *characteristic, ble_gap_conn_desc *desc,
                   uint16_t subValue);
};

// Handle event for characteristic
class CharacteristicCallback : public BLECharacteristicCallbacks {
public:
  void onRead(BLECharacteristic *characteristic);
  void onNotify(BLECharacteristic *characteristic);
  void onSubscribe(BLECharacteristic *characteristic, ble_gap_conn_desc *desc,
                   uint16_t subValue);
};

// Look for advertised device
class AdvertisedClientCallback : public BLEAdvertisedDeviceCallbacks {
private:
  // holds the host information
  BLEAdvertisedDevice host_dev;
  friend class MeshClient;

public:
  void onResult(BLEAdvertisedDevice *other);
};

class ClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient *client);
};

class MeshServer {
private:
  BLEServer *server;
  BLEService *channel_service;
  BLECharacteristic *message_characteristic;

  // handle events for server
  ServerCallback *server_cb;
  // handle events for characteristic
  CharacteristicCallback client_cb;

public:
  MeshServer(Config *config);
};

class MeshClient {
private:
  BLEClient *client;
  AdvertisedClientCallback advertised_cb;
  ClientCallback client_cb;

  static void notify_cb(BLERemoteCharacteristic *remoteCharacteristic,
                        uint8_t *data, size_t length, bool isNotify);

public:
  MeshClient(Config *config);
  void create_client();
  bool connect();
};

class Mesh {
public:
  Mesh();
  Mesh(Config *config);

  void init_esp_now();
  void add_peer(const uint8_t *peer_address);
  void send();
  void send(std::vector<keyswitch_t> &data);
  void wakeup();
  void sleep();
  // void send(KeyData);

  KeyData receive();
  void begin();
  void end();

  static void handle_input(const unsigned char *addr, const uint8_t *data,
                           int len);

  static void send_input(const unsigned char *addr,
                         esp_now_send_status_t status);

  // static KeyData *getBuffer();
  static std::vector<keyswitch_t> *getBuffer();
  static std::vector<keyswitch_t> get_buffer();

  // static KeyData buffer;
  // static std::vector<keyswitch_t> buffer;
  static buffer_t buffer;
  // static std::vector<keyswitch_t> buffer;

private:
  Config *config;

  esp_now_peer_info_t peer;

  MeshServer *server;
  MeshClient *client;
};

void send_input(const unsigned char *addr, esp_now_send_status_t status);
void handle_input(const unsigned char *addr, const uint8_t *data, int len);

#endif
