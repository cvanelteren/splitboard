#ifndef bluetooth_hpp
#define bluetooth_hpp

#include "BLE2902.h"
#include "BLECharacteristic.h"
#include "config.hpp"
#include <BLEClient.h>
#include <BLEDevice.h>
#include <BLEHIDDevice.h>
#include <BLESecurity.h>
#include <BLEServer.h>

#include <HIDTypes.h>

class BleConnection : public BLEServerCallbacks, public BLEClientCallbacks {
public:
  bool connected;

  // only used in client
  BLEAddress *server_adress;

  // server stuff
  BleConnection();
  void onConnect(BLEServer *pServer);
  void onDisconnect(BLEServer *pServer);

  // client stuff
  void onConnect(BLEClient *pClient);
  void onDisconnect(BLEClient *pClient);

  // void onResult(BleAdvertisedDevice device);
  // // Characteristics
  // BLECharacteristic *inputKeyboard;
  // BLECharacteristic *outputKeyboard;
  // BLECharacteristic *inputMediaKeys;

  BLEAdvertising *adv;
};

class Bluetooth {
public:
  BLEServer *server;
  BLEClient *client;
  BLEHIDDevice *hid;
  BLESecurity *security;
  // should hold connection
  BleConnection *connection;
  std::string device_name;
  std::string manufacturer_name;
  std::string server_address;

  // Bluetooth();

  static void taskServer(void *pvParameter);
  static void taskClient(void *pvParameter);

  void onStarted(BLEServer *pServer);
  void onStarted(BLEClient *pClient);

  Bluetooth(Config *config);
  void begin();
};
#endif
