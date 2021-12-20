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

typedef std::vector<keyswitch_t> buffer_t;
typedef std::array<keyswitch_t, 6> message_t;

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
  BLEAdvertisedDevice *host_dev;
  friend class MeshClient;

public:
  void onResult(BLEAdvertisedDevice *other);
};

class Mesh : public BLEServerCallbacks,
             public BLECharacteristicCallbacks,
             public BLEClientCallbacks,
             public BLEAdvertisedDeviceCallbacks {
public:
  Mesh();
  Mesh(Config *config);

  void send();
  void send(std::vector<keyswitch_t> &data);
  void wakeup();
  void sleep();
  // void send(KeyData);

  void begin();
  void end();

  // advertising
  void onResult(BLEAdvertisedDevice *other);
  // client
  // void onRead(BLECharacteristic *characteristic);
  // void onNotify(BLECharacteristic *characteristic);
  // void onSubscribe(BLECharacteristic *characteristic, ble_gap_conn_desc
  // *desc, uint16_t subValue);

  bool connect(BLEClient *client);
  void onDisconnect(BLEClient *client);
  BLEClient *create_client(BLEAdvertisedDevice *host_dev);
  static void notify_cb(BLERemoteCharacteristic *remoteCharacteristic,
                        uint8_t *data, size_t length, bool isNotify);

  // server
  // has no functions as the moment
  // esp now functions
  void init_esp_now();
  void add_peer(const uint8_t *peer_address);
  // server
  static void handle_input(const unsigned char *addr, const uint8_t *data,
                           int len);

  static void send_input(const unsigned char *addr,
                         esp_now_send_status_t status);

  // end esp now

  // external interface to buffers
  static std::vector<keyswitch_t> *getBuffer();
  static std::vector<keyswitch_t> get_buffer();

  static std::vector<keyswitch_t> buffer;

private:
  Config *config;

  bool is_server; // role indicator
  esp_now_peer_info_t peer;

  // store message
  friend class Keyboard;
};

void send_input(const unsigned char *addr, esp_now_send_status_t status);
void handle_input(const unsigned char *addr, const uint8_t *data, int len);

#endif
