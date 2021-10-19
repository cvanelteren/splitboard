#ifndef action_hpp
#define action_hpp

#include <stdint.h>

union Action {
  /**
   * @brief      Keycode actions
   *
   * @details   Differen keyswitches will have various different actions to
   * to perform. This class should hold the various different structures
   *
   * @param      param
   *
   * @return     return type
   */
  uint8_t keycode;

  struct layer_tap {
    uint8_t layer;
  };

  struct mod_tap {
    uint8_t modifier;
  };

  // struct leader_key {};
};
#endif
