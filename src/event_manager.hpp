#include <string>
#include <vector>

#include <Arduino.h>
#include <U8g2lib.h>
#include <freertos/semphr.h>

#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_

class EventManager {
private:
  std::vector<std::string> queue;

public:
  EventManager();
  void add_event(std::string event);
  void update();
  static void start_xtask(void *obj);
  void begin();
};

#endif // EVENT_MANAGER_H_
