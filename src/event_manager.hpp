#include <string>
#include <vector>

#include "display.hpp"
#include <Arduino.h>
#include <U8g2lib.h>
#include <freertos/semphr.h>
#include <memory>
#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

/**
 * @brief  Interface for  the events.
 *
 The  idea is  that  every event  type has  access to  the
 correct object through a pointer. Then  the methods need
 to be written in a way where different event types can be
 created.
 */

// deprecated
// This ended up being a little bit too complicated. It is easier to just
// directly interact with the keyboard by calling an extern promise.
// This is not the prettiest way, but it gets very complicated otherwise.
class AbstractEvent {
public:
  AbstractEvent(std::string name, uint32_t type);
  virtual bool run() = 0;
  std::string get_name();
  uint32_t get_type();

protected:
  std::string name;
  uint32_t type;
};

template <class T> class Event : public AbstractEvent {
public:
  Event(std::string name, uint32_t type, T *component);
  ~Event();

protected:
  T *component;
};

#include "display.hpp"
class DisplayEvent : public Event<Display> {
public:
  bool run() override;
};

#include "led_driver.hpp"
class LEDEvent : public Event<LED> {
public:
  bool run() override;
};
// end deprecated

class EventManager {
private:
  std::vector<bool (*)()> queue;
  // std::vector<AbstractEvent *> queue;

public:
  EventManager();

  // void add_event(AbstractEvent &event);
  void add_event(bool (*event)());
  void update();
  static void start_xtask(void *obj);
  void begin();
};

#endif // EVENT_MANAGER_H
