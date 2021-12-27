#ifndef mesh_hpp
#define mesh_hpp
#include "config.hpp"

#include "types.hpp"
#include <cstdint>

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

// message kinds
/* KIND BIT : event
 * --------------------------
 * 0x0000   : key event down
 * 0x0001   : key event up
 * 0x0010   : one shot event
 * */
typedef union message {
  uint16_t bits;

  struct matrix {
    uint8_t kind : 4;
    uint8_t col : 6;
    uint8_t row : 6;
  };

  struct event {
    uint8_t kind : 4;
    uint8_t event_idx : 8;
    // 4 bits left over
  };

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
  void send(msg_t &msg);

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
