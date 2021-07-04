#include "bluetooth.hpp"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define LOG_TAG ""
#else
#include "esp_log.h"
static const char *LOG_TAG = "BLEDevice";
#endif

// Report IDs:
#define KEYBOARD_ID 0x01
#define MEDIA_KEYS_ID 0x02

// some stuff relating to hid description of functions
// unclear how to read this
// stuff in here is related to various different keys such as ctrl, media keys
// etc
static const uint8_t _hidReportDescriptor[] = {
    USAGE_PAGE(1), 0x01, // USAGE_PAGE (Generic Desktop Ctrls)
    USAGE(1), 0x06,      // USAGE (Keyboard)
    COLLECTION(1), 0x01, // COLLECTION (Application)
    // ------------------------------------------------- Keyboard
    REPORT_ID(1), KEYBOARD_ID, //   REPORT_ID (1)
    USAGE_PAGE(1), 0x07,       //   USAGE_PAGE (Kbrd/Keypad)
    USAGE_MINIMUM(1), 0xE0,    //   USAGE_MINIMUM (0xE0)
    USAGE_MAXIMUM(1), 0xE7,    //   USAGE_MAXIMUM (0xE7)
    LOGICAL_MINIMUM(1), 0x00,  //   LOGICAL_MINIMUM (0)
    LOGICAL_MAXIMUM(1), 0x01,  //   Logical Maximum (1)
    REPORT_SIZE(1), 0x01,      //   REPORT_SIZE (1)
    REPORT_COUNT(1), 0x08,     //   REPORT_COUNT (8)
    HIDINPUT(1), 0x02,         //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred
                               //   State,No Null Position)
    REPORT_COUNT(1), 0x01,     //   REPORT_COUNT (1) ; 1 byte (Reserved)
    REPORT_SIZE(1), 0x08,      //   REPORT_SIZE (8)
    HIDINPUT(1), 0x01,      //   INPUT (Const,Array,Abs,No Wrap,Linear,Preferred
                            //   State,No Null Position)
    REPORT_COUNT(1), 0x05,  //   REPORT_COUNT (5) ; 5 bits (Num lock, Caps lock,
                            //   Scroll lock, Compose, Kana)
    REPORT_SIZE(1), 0x01,   //   REPORT_SIZE (1)
    USAGE_PAGE(1), 0x08,    //   USAGE_PAGE (LEDs)
    USAGE_MINIMUM(1), 0x01, //   USAGE_MINIMUM (0x01) ; Num Lock
    USAGE_MAXIMUM(1), 0x05, //   USAGE_MAXIMUM (0x05) ; Kana
    HIDOUTPUT(1), 0x02,     //   OUTPUT (Data,Var,Abs,No Wrap,Linear,Preferred
                            //   State,No Null Position,Non-volatile)
    REPORT_COUNT(1), 0x01,  //   REPORT_COUNT (1) ; 3 bits (Padding)
    REPORT_SIZE(1), 0x03,   //   REPORT_SIZE (3)
    HIDOUTPUT(1), 0x01,    //   OUTPUT (Const,Array,Abs,No Wrap,Linear,Preferred
                           //   State,No Null Position,Non-volatile)
    REPORT_COUNT(1), 0x06, //   REPORT_COUNT (6) ; 6 bytes (Keys)
    REPORT_SIZE(1), 0x08,  //   REPORT_SIZE(8)
    LOGICAL_MINIMUM(1), 0x00, //   LOGICAL_MINIMUM(0)
    LOGICAL_MAXIMUM(1), 0x65, //   LOGICAL_MAXIMUM(0x65) ; 101 keys
    USAGE_PAGE(1), 0x07,      //   USAGE_PAGE (Kbrd/Keypad)
    USAGE_MINIMUM(1), 0x00,   //   USAGE_MINIMUM (0)
    USAGE_MAXIMUM(1), 0x65,   //   USAGE_MAXIMUM (0x65)
    HIDINPUT(1), 0x00, //   INPUT (Data,Array,Abs,No Wrap,Linear,Preferred
                       //   State,No Null Position)
    END_COLLECTION(0), // END_COLLECTION
    // ------------------------------------------------- Media Keys
    USAGE_PAGE(1), 0x0C,         // USAGE_PAGE (Consumer)
    USAGE(1), 0x01,              // USAGE (Consumer Control)
    COLLECTION(1), 0x01,         // COLLECTION (Application)
    REPORT_ID(1), MEDIA_KEYS_ID, //   REPORT_ID (3)
    USAGE_PAGE(1), 0x0C,         //   USAGE_PAGE (Consumer)
    LOGICAL_MINIMUM(1), 0x00,    //   LOGICAL_MINIMUM (0)
    LOGICAL_MAXIMUM(1), 0x01,    //   LOGICAL_MAXIMUM (1)
    REPORT_SIZE(1), 0x01,        //   REPORT_SIZE (1)
    REPORT_COUNT(1), 0x10,       //   REPORT_COUNT (16)
    USAGE(1), 0xB5,              //   USAGE (Scan Next Track)     ; bit 0: 1
    USAGE(1), 0xB6,              //   USAGE (Scan Previous Track) ; bit 1: 2
    USAGE(1), 0xB7,              //   USAGE (Stop)                ; bit 2: 4
    USAGE(1), 0xCD,              //   USAGE (Play/Pause)          ; bit 3: 8
    USAGE(1), 0xE2,              //   USAGE (Mute)                ; bit 4: 16
    USAGE(1), 0xE9,              //   USAGE (Volume Increment)    ; bit 5: 32
    USAGE(1), 0xEA,              //   USAGE (Volume Decrement)    ; bit 6: 64
    USAGE(2), 0x23, 0x02,        //   Usage (WWW Home)            ; bit 7: 128
    USAGE(2), 0x94, 0x01,        //   Usage (My Computer) ; bit 0: 1
    USAGE(2), 0x92, 0x01,        //   Usage (Calculator)  ; bit 1: 2
    USAGE(2), 0x2A, 0x02,        //   Usage (WWW fav)     ; bit 2: 4
    USAGE(2), 0x21, 0x02,        //   Usage (WWW search)  ; bit 3: 8
    USAGE(2), 0x26, 0x02,        //   Usage (WWW stop)    ; bit 4: 16
    USAGE(2), 0x24, 0x02,        //   Usage (WWW back)    ; bit 5: 32
    USAGE(2), 0x83, 0x01,        //   Usage (Media sel)   ; bit 6: 64
    USAGE(2), 0x8A, 0x01,        //   Usage (Mail)        ; bit 7: 128
    HIDINPUT(1), 0x02, //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred
                       //   State,No Null Position)
    END_COLLECTION(0)  // END_COLLECTION
};

// connection
// BleConnection::BleConnection() {}

BleConnection::BleConnection() { this->connected = false; }
void BleConnection::onConnect(BLEServer *pServer) {
  this->connected = true;

  this->adv->start();
  // BLE2902 *desc = (BLE2902 *)this->inputKeyboard->getDescriptorByUUID(
  //     BLEUUID((uint16_t)0x2902));
  // desc->setNotifications(true);

  // desc = (BLE2902 *)this->inputMediaKeys->getDescriptorByUUID(
  //     BLEUUID((uint16_t)0x2902));
  // desc->setNotifications(true);
}

void BleConnection::onDisconnect(BLEServer *pServer) {
  this->connected = false;
  this->adv->start();

  // BLE2902 *desc = (BLE2902 *)this->inputKeyboard->getDescriptorByUUID(
  //     BLEUUID((uint16_t)0x2902));
  // desc->setNotifications(false);

  // desc = (BLE2902 *)this->inputMediaKeys->getDescriptorByUUID(
  //     BLEUUID((uint16_t)0x2902));
  // desc->setNotifications(false);
  // pAdvertising->start();
}

// holds bluetooth
// Bluetooth::Bluetooth(){};
Bluetooth::Bluetooth(Config *config) {
  // setup bluetooth
  // this->config = config;
  this->device_name = config->device_name;
  this->manufacturer_name = config->device_manufacturer;
  this->connection = new BleConnection();

  this->server = NULL;
  this->client = NULL;
  this->hid = NULL;
};

// task server is passed on by begin
void Bluetooth::taskServer(void *pvParameter) {
  // initialize connection
  Bluetooth *ble = (Bluetooth *)pvParameter;
  Serial.println("Setting up connection");
  // bledevice is a static class; this means there are no copies of this class
  // and all the funcitons exist in a public namespace(!)
  // this bledevice essentially holds either a client or a server.
  Serial.println("Initializing ble");
  BLEDevice::init(ble->device_name);

  // setup server
  if (ble->server == NULL) {
    ble->server = BLEDevice::createServer();
    ble->server->setCallbacks(ble->connection);
  }

  // setup security
  if (ble->security == NULL) {
    ble->security = new BLESecurity();
    ble->security->setAuthenticationMode(ESP_LE_AUTH_BOND);
  }

  // // setup hid
  if (ble->hid == NULL) {
    ble->hid = new BLEHIDDevice(ble->server);
    ble->hid->manufacturer()->setValue(ble->manufacturer_name);

    // magic numbers related to specific characteristics?
    ble->hid->pnp(0x02, 0xe502, 0xa111, 0x210);
    ble->hid->hidInfo(0x00, 0x01);

    // setup report map?
    ble->hid->reportMap((uint8_t *)_hidReportDescriptor,
                        sizeof(_hidReportDescriptor));

    ble->hid->startServices();
  }

  Serial.println("Starting services");
  // currently empty
  ble->onStarted(ble->server);

  // start advertising
  ble->connection->adv = ble->server->getAdvertising();
  ble->connection->adv->setAppearance(HID_KEYBOARD);
  ble->connection->adv->addServiceUUID(ble->hid->hidService()->getUUID());

  ble->connection->adv->setScanResponse(false);
  ble->connection->adv->start();
  ESP_LOGD(LOG_TAG, "Advertising started");
  vTaskDelay(portMAX_DELAY);
}

void BleConnection::onConnect(BLEClient *pClient) { this->connected = true; }
void BleConnection::onDisconnect(BLEClient *pClient) {
  this->connected = false;
  // this->adv->start();
}

void Bluetooth::taskClient(void *pvParameter) {

  // initialize connection
  Bluetooth *ble = (Bluetooth *)pvParameter;
  Serial.println("Setting up connection");
  // bledevice is a static class; this means there are no copies of this
  // and all the funcitons exist in a public namespace(!)
  // this bledevice essentially holds either a client or a server.
  Serial.println("Initializing ble");
  BLEDevice::init(ble->device_name);

  // setup server
  if (ble->client == NULL) {
    ble->client = BLEDevice::createClient();
    ble->onStarted(ble->client);
    ble->client->setClientCallbacks(ble->connection);
  }

  // setup security
  if (ble->security == NULL) {
    ble->security = new BLESecurity();
    ble->security->setAuthenticationMode(ESP_LE_AUTH_BOND);
  }

  // // setup hid
  // if (ble->hid == NULL) {
  //   ble->hid = new BLEHIDDevice(ble->client);
  //   ble->hid->manufacturer()->setValue(ble->manufacturer_name);
  //   // magic numbers related to specific characteristics?
  //   ble->hid->pnp(0x02, 0xe502, 0xa111, 0x210);
  //   ble->hid->hidInfo(0x00, 0x01);

  //   // setup report map?
  //   ble->hid->reportMap((uint8_t *)_hidReportDescriptor,
  //                       sizeof(_hidReportDescriptor));

  //   ble->hid->startServices();
  //   Serial.println("Starting services");
  //   // currently empty
  // }

  // ble->connection->adv->setAppearance(HID_KEYBOARD);
  // ble->connection->adv->addServiceUUID(ble->hid->hidService()->getUUID());

  ESP_LOGD(LOG_TAG, "Advertising started");
  vTaskDelay(portMAX_DELAY);
}

void Bluetooth::begin() {
  // xTaskCreate(this->taskServer, "server", 2048, (void *)this, 5, NULL);
  xTaskCreate(this->taskClient, "client", 2048, (void *)this, 5, NULL);
}

// virtual in the original; does not hing
// check whether its is connected to a client
// if the connection is to a client, turn the advertising back on

void Bluetooth::onStarted(BLEServer *pServer) {}

// setup stuff for client
void Bluetooth::onStarted(BLEClient *pClient) {
  BLEScan *scan;
  BLEScanResults scan_results;
  BLEAdvertisedDevice device;
  // while (not this->connection->connected) {
  if (not this->connection->connected) {
    scan = BLEDevice::getScan();
    // scan->setAdvertisedDeviceCallBacks(this->connection->onResult);
    // scan can access private vars of scna results
    // scan for 1 second
    scan->setActiveScan(true);
    scan_results = scan->start(1);
    // check if server is in range
    for (auto idx = 0; idx < scan_results.getCount(); idx++) {
      device = scan_results.getDevice(idx);

      // Serial.println("Target is ");
      // Serial.println(this->server_address.c_str());

      Serial.print("Found device ");
      Serial.println(device.getName().c_str());
      Serial.println(device.getAddress().toString().c_str());

      if (device.getAddress().toString() == this->server_address) {
        pClient->connect(this->server_address);
        Serial.println("Connecting to server");
        break;
      }
    }
  }
}
