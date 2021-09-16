#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_
#include <string>
#include <vector>

class EventManager {
private:
  std::vector<std::string> queue;

public:
  EventManager();
  void add_event(std::string event);
  void update();
  static void startx(void *obj);
  void begin();
};

#endif // EVENT_MANAGER_H_
