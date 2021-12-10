#ifndef UNIT_TEST

#define USE_NIMBLE

// #if defined(USE_NIMBLE)
// #include <NimBLEDevice.h>
// #include <NimBLEHIDDevice.h>
// #include <NimBLEServer.h>
// #include <NimBLEUtils.h>
// #else
// #include "BLE2902.h"
// #include "BLEHIDDevice.h"
// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLEUtils.h>
// #endif // USE_NIMBLE

#include <Arduino.h>
#include <stdint.h>

#include <BleKeyboard.h>

#include <NimBLEDevice.h>

// #include <NimBLEUUID.h>

#include <NimBLECharacteristic.h>

#include <NimBLEAdvertisedDevice.h>

#include <NimBLEUUID.h>

#include <NimBLEClient.h>
#include <NimBLERemoteCharacteristic.h>

#include <iostream>
#include <string>
using namespace std;

// uuid for "private channel"
const char *split_channel_service_uuid = "ee583eec-576b-11ec-bf63-0242ac130002";
// const char *channel_uuid = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char *split_message_uuid = "ee58419e-576b-11ec-bf63-0242ac130002";
// const char *message_uuid = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
//
typedef struct some_test {
  uint8_t x;
  int y;
} some_test_t;

void notifyCB(NimBLERemoteCharacteristic *pRemoteCharacteristic, uint8_t *pData,
              size_t length, bool isNotify) {
  std::string str = (isNotify == true) ? "Notification" : "Indication";
  str += " from ";
  /** NimBLEAddress and NimBLEUUID have std::string operators */
  str += std::string(
      pRemoteCharacteristic->getRemoteService()->getClient()->getPeerAddress());
  str += ": Service = " +
         std::string(pRemoteCharacteristic->getRemoteService()->getUUID());
  str += ", Characteristic = " + std::string(pRemoteCharacteristic->getUUID());

  some_test_t tmp;
  memcpy(&tmp, pData, length);
  // str += ", Value = " + std::string((char *)pData, length);
  printf("VALUE: %d, %d\n", tmp.x, tmp.y);
  Serial.println(str.c_str());
  // str += ", Value = " + std::string(tmp.x);
  // str += ", Value = " + std::string(tmp.y);
  // Serial.println(str.c_str());
}

class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer *pServer) {
    Serial.println("Client connected");
    Serial.println("Multi-connect support: start advertising");
    NimBLEDevice::startAdvertising();
  };
  /** Alternative onConnect() method to extract details of the connection.
   *  See: src/ble_gap.h for the details of the ble_gap_conn_desc struct.
   */
  void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc) {
    Serial.print("Client address: ");
    Serial.println(NimBLEAddress(desc->peer_ota_addr).toString().c_str());
    /** We can use the connection handle here to ask for different connection
     * parameters. Args: connection handle, min connection interval, max
     * connection interval latency, supervision timeout. Units; Min/Max
     * Intervals: 1.25 millisecond increments. Latency: number of intervals
     * allowed to skip. Timeout: 10 millisecond increments, try for 5x interval
     * time for best results.
     */
    pServer->updateConnParams(desc->conn_handle, 24, 48, 0, 60);
  };
  void onDisconnect(NimBLEServer *pServer) {
    Serial.println("Client disconnected - start advertising");
    NimBLEDevice::startAdvertising();
  };
  void onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc) {
    Serial.printf("MTU updated: %u for connection ID: %u\n", MTU,
                  desc->conn_handle);
  };

  /********************* Security handled here **********************
  ****** Note: these are the same return values as defaults ********/
  uint32_t onPassKeyRequest() {
    Serial.println("Server Passkey Request");
    /** This should return a random 6 digit number for security
     *  or make your own static passkey as done here.
     */
    return 123456;
  };

  bool onConfirmPIN(uint32_t pass_key) {
    Serial.print("The passkey YES/NO number: ");
    Serial.println(pass_key);
    /** Return false if passkeys don't match. */
    return true;
  };

  void onAuthenticationComplete(ble_gap_conn_desc *desc) {
    /** Check that encryption was successful, if not we disconnect the client */
    if (!desc->sec_state.encrypted) {
      NimBLEDevice::getServer()->disconnect(desc->conn_handle);
      Serial.println("Encrypt connection failed - disconnecting client");
      return;
    }
    Serial.println("Starting BLE work!");
  };
};

/** Handler class for characteristic actions */
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
  void onRead(NimBLECharacteristic *pCharacteristic) {
    Serial.print(pCharacteristic->getUUID().toString().c_str());
    Serial.print(": onRead(), value: ");
    Serial.println(pCharacteristic->getValue().c_str());
  };

  void onWrite(NimBLECharacteristic *pCharacteristic) {
    Serial.print(pCharacteristic->getUUID().toString().c_str());
    Serial.print(": onWrite(), value: ");
    Serial.println(pCharacteristic->getValue().c_str());
  };
  /** Called before notification or indication is sent,
   *  the value can be changed here before sending if desired.
   */
  void onNotify(NimBLECharacteristic *pCharacteristic) {
    Serial.println("Sending notification to clients");
  };

  /** The status returned in status is defined in NimBLECharacteristic.h.
   *  The value returned in code is the NimBLE host return code.
   */
  void onStatus(NimBLECharacteristic *pCharacteristic, Status status,
                int code) {
    String str = ("Notification/Indication status code: ");
    str += status;
    str += ", return code: ";
    str += code;
    str += ", ";
    str += NimBLEUtils::returnCodeToString(code);
    Serial.println(str);
  };

  void onSubscribe(NimBLECharacteristic *pCharacteristic,
                   ble_gap_conn_desc *desc, uint16_t subValue) {
    String str = "Client ID: ";
    str += desc->conn_handle;
    str += " Address: ";
    str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
    if (subValue == 0) {
      str += " Unsubscribed to ";
    } else if (subValue == 1) {
      str += " Subscribed to notfications for ";
    } else if (subValue == 2) {
      str += " Subscribed to indications for ";
    } else if (subValue == 3) {
      str += " Subscribed to notifications and indications for ";
    }
    str += std::string(pCharacteristic->getUUID()).c_str();

    Serial.println(str);
  };
};

BleKeyboard ble;
BLEServer *server;
BLEAdvertising *advertising;
// NimBLEClient *client;
NimBLEService *channel_service;
NimBLECharacteristic *message_characteristic;

NimBLEAdvertisedDevice *host_dev;

int scanTime = 1; // In seconds
NimBLEScan *scan;

bool is_scanner;
bool scanning = 1;
const char *host_addr = "7c:9e:bd:fb:d9:7a";

static CharacteristicCallbacks chrCallbacks;

/** Callback to process the results of the last scan or restart it */
void scanEndedCB(NimBLEScanResults results) { Serial.println("Scan Ended"); }

class ClientCallbacks : public NimBLEClientCallbacks {
  void onConnect(NimBLEClient *pClient) {
    Serial.println("Connected");
    /** After connection we should change the parameters if we don't need fast
     * response times. These settings are 150ms interval, 0 latency, 450ms
     * timout. Timeout should be a multiple of the interval, minimum is 100ms.
     *  I find a multiple of 3-5 * the interval works best for quick
     * response/reconnect. Min interval: 120 * 1.25ms = 150, Max interval: 120
     * * 1.25ms = 150, 0 latency, 60 * 10ms = 600ms timeout
     */
    pClient->updateConnParams(120, 120, 0, 60);
  };

  void onDisconnect(NimBLEClient *pClient) {
    Serial.print(pClient->getPeerAddress().toString().c_str());
    Serial.println(" Disconnected - Starting scan");
    NimBLEDevice::getScan()->start(scanTime, scanEndedCB);
  };

  /** Called when the peripheral requests a change to the connection parameters.
   *  Return true to accept and apply them or false to reject and keep
   *  the currently used parameters. Default will return true.
   */
  bool onConnParamsUpdateRequest(NimBLEClient *pClient,
                                 const ble_gap_upd_params *params) {
    if (params->itvl_min < 24) { /** 1.25ms units */
      return false;
    } else if (params->itvl_max > 40) { /** 1.25ms units */
      return false;
    } else if (params->latency > 2) { /** Number of intervals allowed to skip */
      return false;
    } else if (params->supervision_timeout > 100) { /** 10ms units */
      return false;
    }

    return true;
  };

  /********************* Security handled here **********************
  ****** Note: these are the same return values as defaults ********/
  uint32_t onPassKeyRequest() {
    Serial.println("Client Passkey Request");
    /** return the passkey to send to the server */
    return 123456;
  };

  bool onConfirmPIN(uint32_t pass_key) {
    Serial.print("The passkey YES/NO number: ");
    Serial.println(pass_key);
    /** Return false if passkeys don't match. */
    return true;
  };

  /** Pairing process complete, we can check the results in ble_gap_conn_desc */
  void onAuthenticationComplete(ble_gap_conn_desc *desc) {
    if (!desc->sec_state.encrypted) {
      Serial.println("Encrypt connection failed - disconnecting");
      /** Find the client with the connection handle provided in desc */
      NimBLEDevice::getClientByID(desc->conn_handle)->disconnect();
      return;
    }
  };
};

static ClientCallbacks clientCB;

// everythign works with callbacks
class AdvertisedCB : public NimBLEAdvertisedDeviceCallbacks {

  void onResult(NimBLEAdvertisedDevice *other) override {
    if (other->isAdvertisingService(NimBLEUUID(split_channel_service_uuid))) {
      printf("Found other service!\n");
      printf("Found device %s \n", other->toString().c_str());
      NimBLEDevice::getScan()->stop();
      scanning = false;
      host_dev = other;
    }
  }
};

bool connectToServer() {
  NimBLEClient *client = nullptr;

  // check for existing clients
  if (NimBLEDevice::getClientListSize()) {
    printf("Client size list %d\n", NimBLEDevice::getClientListSize());
    client = NimBLEDevice::getClientByPeerAddress(host_dev->getAddress());
    if (client) {
      if (!(client->connect(host_dev, false))) {
        printf("Reconnect failed\n");
        return false;
      }
      printf("Reconnected to client");
    } else {
      client = NimBLEDevice::getDisconnectedClient();
    }
  }

  printf("Reached here\n");

  if (!client) {
    if (NimBLEDevice::getClientListSize() >= NIMBLE_MAX_CONNECTIONS) {
      printf("Max client size reached - no more connections available\n");
      return false;
    }

    printf("Creating client\n");
    client = NimBLEDevice::createClient();
    printf("New client created\n");

    printf("Setting connection parameters\n");
    client->setClientCallbacks(&clientCB, false);
    client->setConnectionParams(12, 12, 0, 51);
    client->setConnectTimeout(5);

    if (!client->connect(host_dev)) {
      NimBLEDevice::deleteClient(client);
      printf("Failed to connect, deleted client\n");
      return false;
    }
  }

  if (!client->isConnected()) {
    if (!client->connect(host_dev, false)) {
      printf("Failed to connect \n");
      return false;
    }
  }
  printf("Testing here");

  printf("Connected to: %s\n", client->getPeerAddress().toString().c_str());
  printf("SSRI: %d \n", client->getRssi());

  NimBLERemoteService *rSvc;
  NimBLERemoteCharacteristic *rChr;
  NimBLERemoteDescriptor *rDsc;

  rSvc = client->getService(split_channel_service_uuid);
  if (rSvc) {
    rChr = rSvc->getCharacteristic(split_message_uuid);
    if (rChr && rChr->canRead()) {
      some_test_t read = rChr->readValue<some_test_t>();
      printf("Value is: %d, %d\n", read.x, read.y);
      printf("-------------------------------\n");
      printf("-------------------------------\n");
    }

    if (rChr->canNotify()) {
      // if(!pChr->registerForNotify(notifyCB)) {
      if (!rChr->subscribe(true, notifyCB)) {
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

class ShowResults : public NimBLEClientCallbacks {};

some_test_t test = {1, -100};
void setup() {
  Serial.begin(115200);

  std::string name_tag;
  NimBLEDevice::init("");
  if (NimBLEDevice::toString() != host_addr) {
    name_tag = "scanner";
    is_scanner = true;
    NimBLEDevice::init(name_tag);
    printf("Address %s \n", NimBLEDevice::toString().c_str());
    // client =
    // NimBLEDevice::createClient(NimBLEAddress("7c:9e:bd:fb:d9:7a"));
    scan = NimBLEDevice::getScan();
    scan->setAdvertisedDeviceCallbacks(new AdvertisedCB());
    scan->setActiveScan(true);
    scan->setInterval(100);
    scan->setWindow(99);
    scan->start(scanTime, &scanEndedCB);

  } else {
    name_tag = "server";
    NimBLEDevice::init(name_tag);
    printf("Address %s \n", NimBLEDevice::toString().c_str());

    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEDevice::setSecurityPasskey(123456);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);

    server = NimBLEDevice::createServer();
    server->setCallbacks(new ServerCallbacks());
    server->advertiseOnDisconnect(true);
    // server = NimBLEDevice::getServer();
    channel_service = server->createService(split_channel_service_uuid);
    // channel_service = server->createService("0x2A60");

    // make bluetooth variable
    message_characteristic = channel_service->createCharacteristic(
        split_message_uuid, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE |
                                NIMBLE_PROPERTY::NOTIFY);
    message_characteristic->setCallbacks(&chrCallbacks);

    message_characteristic->setValue((uint8_t *)&test, sizeof(test));
    // message_characteristic->notify(1);
    // message_characteristic->start();
    channel_service->start();

    // adverise
    advertising = server->getAdvertising();
    advertising->addServiceUUID(split_channel_service_uuid);
    advertising->setScanResponse(true);
    advertising->setMinPreferred(
        0x06); // functions that help with iPhone connections issue
    advertising->setMinPreferred(0x12);
    server->startAdvertising();
  }
}

static size_t counter;
void loop() {
  // printf("NUmber of services %d\r", client->getServices()->size());
  if (is_scanner) {
    while (scanning) {
      delay(100);
    }

    // scanning = true;
    try {
      if (connectToServer()) {
        printf("Succes\n");
      } // else {
      NimBLEDevice::getScan()->start(scanTime, scanEndedCB);

    } catch (const std::exception &e) {
      printf("%s", e.what());
    }

  } else {
    char buff[128];
    if (server->getConnectedCount()) {
      NimBLEService *tmp = server->getServiceByUUID(split_channel_service_uuid);
      if (tmp) {
        NimBLECharacteristic *chr = tmp->getCharacteristic(split_message_uuid);
        // sprintf(buff, "%d\n", counter);
        test.x++;
        test.y++;
        if (chr) {
          chr->setValue((uint8_t *)&test, sizeof(test));
          counter++;
          chr->notify(true);
        }
      }
    }
    delay(500);
  }
}

#ifdef PRORDUCTION
#include "esp32-hal-cpu.h"
#include <WiFi.h>
#include <esp_now.h>
#include <string>

#include "config.hpp"
#include "keyboard.hpp"

#include "key_definitions.hpp"
#include <esp_pm.h>

#include "event_manager.hpp"

// #include <BLEDevice.h>
// #include <BLEServer.h>

Config config = Config();
Keyboard keyboard = Keyboard(&config);
EventManager manager = EventManager();

// esp_pm_config_esp32 *config_pm;
void setup() {
  setCpuFrequencyMhz(240);
  pinMode(config.batt_pin, ANALOG);
  // esp_pm_get_configuration(config_pm);

  // this needs to be here for some reason?
  WiFi.mode(WIFI_STA);
  Serial.begin(config.baud_rate);

  keyboard.begin();
  manager.begin();

  keyboard.led->active_keys = &(keyboard.matrix->active_keys);

  printf("------------SPLITBOARD------------\n");
  // printf("layers size %d\n", keyboard.layers.size());
  printf("I am server? %d", keyboard.is_server);
}

void loop() {
  static bool ble_connected;

  keyboard.update();
  keyboard.led->update();
  // // manager.add_event("display");
  // if (keyboard.bluetooth.isConnected()) {
  //   // esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL0, ESP_PWR_LVL_N11);
  //   if (!(ble_connected)) {
  //     // manager.add_event("display");
  //   }

  //   ble_connected = 1;

  // } else {
  //   ble_connected = 0;
  // }
  // test_manager();
  // keyboard.led->update();

  // tmp stuff
}
#endif
#endif
