#ifndef mesh_hpp
#define mesh_hpp
#include "config.hpp"

#include "types.hpp"

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

// deprecated
typedef struct {
  uint16_t keys;
  uint8_t layer;

} msg_t;

typedef std::vector<keyswitch_t> buffer_t;
typedef std::array<keyswitch_t, 6> message_t;
// end deprecated

class Mesh : public BLEServerCallbacks,
             public BLECharacteristicCallbacks,
             public BLEClientCallbacks,
             public BLEAdvertisedDeviceCallbacks {
public:
  Mesh(Config *config);
  void send(std::vector<keyswitch_t> &data);
  void wakeup();
  void sleep();

  void begin();
  void end();

  // advertising
  void onResult(BLEAdvertisedDevice *other) override;
  // client
  // void onRead(BLECharacteristic *characteristic);
  // void onNotify(BLECharacteristic *characteristic);
  // void onSubscribe(BLECharacteristic *characteristic, ble_gap_conn_desc
  // *desc, uint16_t subValue);

  void scan();
  bool connect_to_server(BLEClient *client);
  void onDisconnect(BLEClient *client) override;
  void onConnect(BLEClient *client) override;
  bool onConfirmPIN(uint32_t pass_key) override;

  BLEClient *create_client(BLEAdvertisedDevice *host_dev);
  static void notify_cb(BLERemoteCharacteristic *remoteCharacteristic,
                        uint8_t *data, size_t length, bool isNotify);

  // external interface to buffers
  static std::vector<keyswitch_t> get_buffer();
  static std::vector<keyswitch_t> buffer;

private:
  Config *config;
  BLEService *channel_service;
  BLECharacteristic *message_characteristic;

  bool is_server; // role indicator

  // store message
  friend class Keyboard;
};

#endif
