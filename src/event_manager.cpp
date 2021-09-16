#include "event_manager.hpp"
#include "keyboard.hpp"
#include <Arduino.h>

#include <U8g2lib.h>
extern Keyboard keyboard;
EventManager::EventManager() { queue = std::vector<std::string>(); }
void EventManager::add_event(std::string event) { queue.push_back(event); }

void EventManager::update() {
  static uint8_t idx;
  static bool update_battery;
  char test[] = "abcdefghijklmnopqrstuvw";
  // deal with events

  // keyboard.display->setFont(u8g2_font_open_iconic_all_1x_t);
  // keyboard.display->setPowerSave(0);

  for (auto event : queue) {
    if (event == "display") {
      Serial.println(event.c_str());
      Serial.println(test[idx]);

      keyboard.display->firstPage();
      do {
        keyboard.display->setFont(u8g2_font_open_iconic_embedded_2x_t);
        keyboard.display->drawStr(10, 50, "\x40");
        keyboard.display->setFont(u8g2_font_tom_thumb_4x6_mf);
        keyboard.display->log.print(test[idx]);
        keyboard.display->log.print("\rhello:)");
      } while (keyboard.display->nextPage());
      idx++;
      idx %= sizeof(test);
    }

    //   switch (event) {
    //   case "display":
    //     break;
    //   default:
    //     break;
    //   }
  }
  queue.clear();
}

void EventManager::startx(void *obj) {
  // recast and run

  EventManager *tmp = (EventManager *)obj;
  Serial.printf("hello from core %d\n", xPortGetCoreID());
  while (true) {
    tmp->update();
  }
}

void EventManager::begin() {
  xTaskCreatePinnedToCore(this->startx, "event_manager", 2048, (void *)this, 1,
                          NULL, 1);
}
