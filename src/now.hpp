#ifndef now_hpp
#define now_hpp
#include "config.hpp"

#include <WiFi.h>
#include <esp_now.h>
class EspNow {
public:
  EspNow();
  EspNow(Config *config);

  void init_esp_now();
  void register_send_callback(esp_now_send_cb_t func);
  void register_receive_callback(esp_now_recv_cb_t func);
  void add_peer(const uint8_t *peer_address);
};

void send_input(const unsigned char *addr, esp_now_send_status_t status);
void handle_input(const unsigned char *addr, const uint8_t *data, int len);
#endif
