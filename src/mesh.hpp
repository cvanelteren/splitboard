#ifndef mesh_hpp
#define mesh_hpp
#include "config.hpp"

#include "types.h"
#include <WiFi.h>
#include <esp_now.h>
class Mesh {
public:
  Mesh();
  Mesh(Config *config);

  void init_esp_now();
  void register_send_callback(esp_now_send_cb_t func);
  void register_receive_callback(esp_now_recv_cb_t func);
  void add_peer(const uint8_t *peer_address);
  void send(KeyData data);

  KeyData receive();

  static void handle_input(const unsigned char *addr, const uint8_t *data,
                           int len);

  static void send_input(const unsigned char *addr,
                         esp_now_send_status_t status);

  static KeyData *getBuffer();

  static KeyData buffer;

private:
  Config *config;
};

void send_input(const unsigned char *addr, esp_now_send_status_t status);
void handle_input(const unsigned char *addr, const uint8_t *data, int len);

#endif
