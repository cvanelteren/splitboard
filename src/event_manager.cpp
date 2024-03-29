#include "event_manager.hpp"

// #include <FreeRTOS.h>
// #include "keyboard.hpp"
// extern Keyboard keyboard;
//
static SemaphoreHandle_t mutex;

/*
 * What do I want to achieved?
 * Running a piece of code that can access properties of the keyboard.
 * For example, setting hte brightness of the leds,
 * changing the colors, displaying text on the screen such as
 * battery level.
 *
 * Especially for the colors, they need to be communicated over the mesh.
 * The keyboard therefore seems sensible for the events to act on.
 * This allows for free access of all the properties.
 *
 */
EventManager::EventManager() { mutex = xSemaphoreCreateMutex(); }

void EventManager::add_event(bool (*event)()) {
  if (xSemaphoreTake(mutex, 0) == pdTRUE) {
    // queue.push_back(&event));
    queue.push_back(event);
    xSemaphoreGive(mutex);
  }
}

void EventManager::update() {
  // char test[] = "abcdefghijklmnopqrstuvw";
  // AbstractEvent *event;

  // deal with events
  if (xSemaphoreTake(mutex, 0) == pdTRUE) {
    // printf("Q:\t%d\n", queue.size());
    while (queue.size()) {
      // event = queue.back().run();
      auto event = queue.back();
      if (event()) {
        printf("Event failed\n");
      }
      queue.pop_back();
      // if (event == "display") {
      //   // emulate dummy data for now
      //   idx++;
      //   idx %= sizeof(test);
      //   keyboard.display->setCursor(10, 5);

      //   keyboard.display->firstPage();
      // ) {
      //   //   keyboard.display->log.println();
      //   // }

      //   do {

      //     //
      //     keyboard.display->setFont(u8g2_font_open_iconic_embedded_2x_t);
      //     keyboard.display->setFont(u8g2_font_tom_thumb_4x6_mf);
      //     auto x = keyboard.get_battery_level();
      //     keyboard.display->log.printf("%0.2f %d %d %d %d\r", x,
      //                                  analogRead(keyboard.config->batt_pin),
      //                                  BAT_MAX_ADC, BAT_MIN_ADC);
      //     // keyboard.display->setCursor(10, 80);
      //     // keyboard.display->drawStr(10, 80, "\x40");
      //     // keyboard.display->setCursor(10, 5);
      //     // keyboard.display->drawLog(10, 5, keyboard.display->log);

      //     // keyboard.display->log.print("\rhello:)");
      //   } while (keyboard.display->nextPage());

      // } else if (event == "led") {
      //   keyboard.led->update();
      // }
    }
    xSemaphoreGive(mutex);
  }
}

void EventManager::start_xtask(void *obj) {
  // recast and run
  EventManager *tmp = (EventManager *)obj;
  Serial.printf("hello from core %d\n", xPortGetCoreID());
  while (true) {
    tmp->update();
  }
}

void EventManager::begin() {
  xTaskCreatePinnedToCore(this->start_xtask, "event_manager", 2048,
                          (void *)this, 1, NULL, 1);
}

AbstractEvent::AbstractEvent(std::string name, uint32_t type) {
  this->name = name;
  this->type = type;
}

std::string AbstractEvent::get_name() { return name; }

uint32_t AbstractEvent::get_type() { return type; }

template <class T>
Event<T>::Event(std::string name, uint32_t type, T *device)
    : AbstractEvent(name, type) {
  this->component = component;
}

template <class T> Event<T>::~Event() { delete component; }

// TODO: implement
bool DisplayEvent::run() {
  return 0;

  // switch (this->get_type()) {
  // case DISPLAY_CENTER_EVENT:
  //   break;
  // case DISPLAY_NOTIFICATION_EVENT:
  //   break;
  // case DISPLAY_BATTERY_EVENT:
  //   break;
  // default:
  //   break;
  // }
  // return 0;
}

bool LEDEvent::run() {
  if (component->set_mode(type)) {
    return true;
  }
  return false; // all good

  // switch (get_type()) {
  // case LED_SERIAL_CYCLE:
  // case LED_FOLLOW_ME:
  // case LED_CYCLE:
  //   return component->set_mode(type);
  //   break;
  // default:
  //   break;
  // }
}
