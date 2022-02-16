#ifndef UNIT_TEST

#include <Arduino.h>
#include <BleKeyboard.h>

#include <iostream>
#include <stdint.h>
#include <string>

#include "esp32-hal-cpu.h"
#include <esp_pm.h>

#include "config.hpp"
#include "event_manager.hpp"
#include "keyboard.hpp"
#include "mesh.hpp"

Config config = Config();
Keyboard keyboard = Keyboard(&config);
// Mesh mesh = Mesh(&config);

BLEServer *server;
BLEAdvertising *advertising;
// NimBLEClient *client;
NimBLEService *channel_service;
NimBLECharacteristic *message_characteristic;
NimBLEAdvertisedDevice *HOST1;
NimBLEScan *scan;

static NimBLECharacteristicCallbacks chrCallbacks;
static NimBLEClientCallbacks clientCB;
extern const char *split_channel_service_uuid;
extern const char *split_message_uuid;
bool scanning = true;

class AdvertisedCB : public NimBLEAdvertisedDeviceCallbacks {

  void onResult(NimBLEAdvertisedDevice *other) override {
    if (other->isAdvertisingService(NimBLEUUID(split_channel_service_uuid))) {
      printf("Found other service!\n");
      printf("Found device %s \n", other->toString().c_str());
      NimBLEDevice::getScan()->stop();
      scanning = false;
      HOST1 = other;
    }
  }
};
void scanEndedCB(NimBLEScanResults results) { Serial.println("Scan Ended"); }

void notifyCB(NimBLERemoteCharacteristic *pRemoteCharacteristic, uint8_t *pData,
              size_t length, bool isNotify) {
  std::string str = (isNotify == true) ? "Notification" : "Indication";
  str += " from ";
  str += std::string(
      pRemoteCharacteristic->getRemoteService()->getClient()->getPeerAddress());
  str += ": Service = " +
         std::string(pRemoteCharacteristic->getRemoteService()->getUUID());
  str += ", Characteristic = " + std::string(pRemoteCharacteristic->getUUID());

  // str += ", Value = " + std::string((char *)pData, length);
  // str += ", Value = " + std::string(tmp.x);
  // str += ", Value = " + std::string(tmp.y);
  // Serial.println(str.c_str());
}

// bool connectToServer(BLEAdvertisedDevice *host_dev) {
//   NimBLEClient *client = nullptr;

//   // check for existing clients
//   if (BLEDevice::getClientListSize()) {
//     printf("Client size list %d\n", BLEDevice::getClientListSize());
//     client = BLEDevice::getClientByPeerAddress(host_dev->getAddress());
//     if (client) {
//       if (!(client->connect(host_dev, false))) {
//         printf("Reconnect failed\n");
//         return false;
//       }
//       printf("Reconnected to client");
//     } else {
//       client = BLEDevice::getDisconnectedClient();
//     }
//   }

//   if (!client) {
//     if (BLEDevice::getClientListSize() >= NIMBLE_MAX_CONNECTIONS) {
//       printf("Max client size reached - no more connections available\n");
//       return false;
//     }

//     printf("Creating client\n");
//     client = BLEDevice::createClient(host_dev->getAddress());
//     printf("New client created\n");

//     printf("Setting connection parameters\n");
//     client->setClientCallbacks(&mesh, false);
//     client->setConnectionParams(12, 12, 0, 51);
//     client->setConnectTimeout(5);
//     printf("Params set\n");

//     if (!client->connect()) {
//       BLEDevice::deleteClient(client);
//       printf("Failed to connect, deleted client\n");
//       return false;
//     }
//   }

//   if (!client->isConnected()) {
//     if (!client->connect(false)) {
//       printf("Failed to connect \n");
//       return false;
//     }
//   }

//   printf("Testing here");

//   printf("Connected to: %s\n", client->getPeerAddress().toString().c_str());
//   printf("SSRI: %d \n", client->getRssi());

//   NimBLERemoteService *remote_service;
//   NimBLERemoteCharacteristic *remote_characteristic;
//   NimBLERemoteDescriptor *remote_description;

//   remote_service = client->getService(split_channel_service_uuid);
//   if (remote_service) {
//     remote_characteristic =
//         remote_service->getCharacteristic(split_message_uuid);
//     if (remote_characteristic && remote_characteristic->canRead()) {
//     }

//     if (remote_characteristic->canNotify()) {
//       // if(!pChr->registerForNotify(notifyCB)) {
//       if (!remote_characteristic->subscribe(true, notifyCB)) {
//         client->disconnect();
//         return false;
//       }
//     }

//   } else {
//     printf("Service not found\n");
//   }

//   printf("Done with device!\n");
//   return true;
// }

void setup() {
  pinMode(config.batt_pin, ANALOG);
  Serial.begin(config.baud_rate);

  BLEDevice::init("");
  delay(500);

  // BLEDevice::init("");
  // auto servAddr = BLEAddress(config.serv_add);
  // if (BLEDevice::getAddress().toString().compare(servAddr) == 0) {
  //   BLEDevice::init(config.device_name);
  //   scan = BLEDevice::getScan();
  //   scan->setAdvertisedDeviceCallbacks(new AdvertisedCB());
  //   scan->setActiveScan(true);
  //   scan->setInterval(100);
  //   scan->setWindow(99);
  //   scan->start(2, &scanEndedCB);
  // } else {
  //   BLEDevice::init("Server");
  //   BLEDevice::setSecurityAuth(true, true, true);
  //   BLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
  //   server = BLEDevice::createServer();
  //   server->advertiseOnDisconnect(true);
  //   channel_service = server->createService(split_channel_service_uuid);
  //   message_characteristic = channel_service->createCharacteristic(
  //       split_message_uuid, NIMBLE_PROPERTY::READ |
  //       NIMBLE_PROPERTY::WRITE |
  //                               NIMBLE_PROPERTY::NOTIFY);
  //   message_characteristic->setCallbacks(&mesh);

  //   // message_characteristic->setValue((uint8_t *)&test, sizeof(test));
  //   // message_characteristic->notify(1);
  //   // message_characteristic->start();
  //   channel_service->start();
  //   advertising = server->getAdvertising();
  //   advertising->addServiceUUID(split_channel_service_uuid);
  //   advertising->setScanResponse(true);
  //   advertising->setMinPreferred(
  //       0x06); // functions that help with iPhone connections issue
  //   advertising->setMinPreferred(0x12);
  //   server->startAdvertising();
  // }

  // mesh.begin();
  keyboard.begin();
  keyboard.led->active_keys = &(keyboard.matrix->active_keys);

  printf("------------SPLITBOARD------------\n");
  // printf("layers size %d\n", keyboard.layers.size());
}

static bool is_connected;
void loop() {
  // keyboard.mesh->update();
  keyboard.update();
  // while (!keyboard.mesh->is_connected()) {
  // printf("%s\r", (keyboard.mesh->is_connected() ? "yes" : "no"));
  // keyboard.mesh->connectServer();
  // delay(1000);
  // keyboard.mesh->scan();
  // }

  // keyboard.update();
  // keyboard.led->update();
  // // manager.add_event("display");
  // if (keyboard.bluetooth.isConnected()) {
  //   // esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL0, ESP_PWR_LVL_N11);
  //   if (!(ble_connected)) {
  //     // manager.add_event("display");
  //   }

  // test_manager();
  // keyboard.led->update();

  // tmp stuff
}
#endif
