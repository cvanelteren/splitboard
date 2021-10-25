#ifndef mesh_hpp
#define mesh_hpp
#include "config.hpp"

#include "types.hpp"
#include <WiFi.h>
#include <esp_now.h>

typedef struct {
  uint16_t keys;
  uint8_t layer;

} msg_t;

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
};

void send_input(const unsigned char *addr, esp_now_send_status_t status);
void handle_input(const unsigned char *addr, const uint8_t *data, int len);

#endif
