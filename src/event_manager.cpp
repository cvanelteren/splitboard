#include "event_manager.hpp"
#include "keyboard.hpp"
#include <Arduino.h>

#include <U8g2lib.h>
#include <freertos/semphr.h>

extern Keyboard keyboard;
static SemaphoreHandle_t mutex;
EventManager::EventManager() {
  queue = std::vector<std::string>();
  mutex = xSemaphoreCreateMutex();
}
void EventManager::add_event(std::string event) {
  if (xSemaphoreTake(mutex, 0) == pdTRUE) {
    queue.push_back(event);
    xSemaphoreGive(mutex);
  }
}

void EventManager::update() {
  static uint8_t idx;
  static bool update_battery;

  char test[] = "abcdefghijklmnopqrstuvw";
  std::string event;
  // deal with events
  if (xSemaphoreTake(mutex, 0) == pdTRUE) {
    // printf("Q:\t%d\n", queue.size());
    while (queue.size()) {
      event = queue.back();
      queue.pop_back();
      if (event == "display") {
        // emulate dummy data for now
        idx++;
        idx %= sizeof(test);
        keyboard.display->setCursor(10, 5);
        keyboard.display->log.print(test[idx]);
        keyboard.display->firstPage();
        if (!idx) {
          keyboard.display->log.println();
        }
        do {
          keyboard.display->setFont(u8g2_font_open_iconic_embedded_2x_t);
          keyboard.display->setCursor(10, 80);
          keyboard.display->drawStr(10, 80, "\x40");
          keyboard.display->setFont(u8g2_font_tom_thumb_4x6_mf);
          keyboard.display->setCursor(10, 5);
          keyboard.display->drawLog(10, 5, keyboard.display->log);

          // keyboard.display->log.print("\rhello:)");
        } while (keyboard.display->nextPage());
      } else if (event == "led") {
        keyboard.led->update();
      }
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
