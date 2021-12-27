#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <BleKeyboard.h>
// bluetooth keyboard holding
#include <cstddef>
#include <unordered_map>
#include <utility>

// holds configuration for the entire keyboard
#include "config.hpp"
// internal to readout the pins
#include "matrix.hpp"
// holds layout keys
#include "display.hpp"
#include "event_manager.hpp"
#include "layout.hpp"
#include "led_driver.hpp"
#include "mesh.hpp"
#include "rotary_encoder.hpp"
#include "types.hpp"

// * // controls
#define KC_BSPC 0xB2  // BS	Backspace
#define KC_TAB 0xB3   // TAB	Tab
#define KC_ENTER 0xB0 // LF	Enter
#define KC_ESC 0xB1

// alphabet
#define KC_A 0x61
#define KC_B 0x62
#define KC_C 0x63
#define KC_D 0x64
#define KC_E 0x65
#define KC_F 0x66
#define KC_G 0x67
#define KC_H 0x68
#define KC_I 0x69
#define KC_J 0x6a
#define KC_K 0x6b
#define KC_L 0x6c
#define KC_M 0x6d // d
#define KC_N 0x6e
#define KC_O 0x6f
#define KC_P 0x70
#define KC_Q 0x71
#define KC_R 0x72
#define KC_S 0x73
#define KC_T 0x74
#define KC_U 0x75
#define KC_V 0x76
#define KC_W 0x77
#define KC_X 0x78
#define KC_Y 0x79
#define KC_Z 0x7a

#define KC_LEFT KEY_LEFT_ARROW
#define KC_DOWN KEY_DOWN_ARROW
#define KC_UP KEY_UP_ARROW
#define KC_RIGHT KEY_RIGHT_ARROW

// capitals

#define SHIFT 0x80

// inside blekeyboard.h
#define KC_LCTL KEY_LEFT_CTRL
#define KC_LSHIFT KEY_LEFT_SHIFT
#define KC_ALT KEY_LEFT_ALT
#define KC_ALTGR KEY_RIGHT_ALT
#define KC_SUPER KEY_LEFT_GUI
#define KC_RCTL KEY_RIGHT_CTRL
#define KC_RSHIFT KEY_RIGHT_SHIFT

#define KC_NO 0x00

// numbers
#define KC_0 0x30
#define KC_1 0x31
#define KC_2 0x32
#define KC_3 0x33
#define KC_4 0x34
#define KC_5 0x35
#define KC_6 0x36
#define KC_7 0x37
#define KC_8 0x38
#define KC_9 0x39

// // math-ey
#define KC_EQ 0x3d    // =
#define KC_MINUS 0x2d // -

// // symbols
#define KC_COLON 0x3A  // :
#define KC_SCOLON 0x3B // ;
// #define KC_LT 0x3C       // <
// #define KC_GT 0x3D       // >
#define KC_QUESTION 0x3F // ?
// // #define KC_AT 40         // @o

#define KC_SLASH 0x2F    // /
#define KC_LBRACKET 0x5B // [
#define KC_BSLASH 0x5C   // bslash
#define KC_RBRACKET 0x5D // ]

#define KC_HAT 0x5E        //^
#define KC_BACKTICK 0x60   // `
#define KC_LCBRACKET 0x7B  // {
#define KC_VMID 0x7C       // |
#define KC_RCBRACKET 0x7D  // }
#define KC_TILDE 0x7E      // ~
#define KC_DEL 0x7F        // DEL
#define KC_SPC 0x20        //  ' '
#define KC_EXCLAIM 0x21    // !
#define KC_QUOTE 0x27      // "
#define KC_HASH 0x23       // #
#define KC_DOLLAR 0x24     // $
#define KC_PRCT 0x25       // %
#define KC_AND 0x26        // &
#define KC_SQUOTE 0x27     // '
#define KC_OPAREN 0x28     // (
#define KC_CPAREN 0x29     // )
#define KC_ASTERISK 0x2A   // *
#define KC_PLUS 0x2B       // +
#define KC_COMMA 0x2C      // ,
#define KC_DASH 0x2D       // -
#define KC_DOT 0x2E        // .
#define KC_AT 0x40         // @
#define KC_UNDERSCORE 0x5F // _

// // Keycode modifiers & aliases
// #define LCTL(kc) (KEY_LEFT_CTRL | (kc))
// #define LSFT(kc) (KEY_LEFT_SHIFT | (kc))
// #define LALT(kc) (KEY_LEFT_ALT | (kc))
// #define LGUI(kc) (KEY_LEFT_GUI | (kc))
// #define LOPT(kc) LALT(kc)
// #define LCMD(kc) LGUI(kc)
// #define LWIN(kc) LGUI(kc)
// #define RCTL(kc) (KEY_RIGHT_CTRL | (kc))
// #define RSFT(kc) (KEY_RIGHT_SHIFT | (kc))
// #define RALT(kc) (KEY_RIGHT_ALT | (kc))
// #define RGUI(kc) (KEY_RIGHT_GUI | (kc))
// #define ALGR(kc) RALT(kc)
// #define ROPT(kc) RALT(kc)
// #define RCMD(kc) RGUI(kc)
// #define RWIN(kc) RGUI(kc)

// /* #define HYPR(kc) (QK_LCTL | QK_LSFT | QK_LALT | QK_LGUI | (kc)) */
// /* #define MEH(kc) (QK_LCTL | QK_LSFT | QK_LALT | (kc)) */
// /* #define LCAG(kc) (QK_LCTL | QK_LALT | QK_LGUI | (kc)) */
// /* #define LSG(kc) (QK_LSFT | QK_LGUI | (kc)) */
// /* #define SGUI(kc) LSG(kc) */
// /* #define SCMD(kc) LSG(kc) */
// /* #define SWIN(kc) LSG(kc) */
// /* #define LAG(kc) (QK_LALT | QK_LGUI | (kc)) */
// /* #define RSG(kc) (QK_RSFT | QK_RGUI | (kc)) */
// /* #define RAG(kc) (QK_RALT | QK_RGUI | (kc)) */
// /* #define LCA(kc) (QK_LCTL | QK_LALT | (kc)) */
// /* #define LSA(kc) (QK_LSFT | QK_LALT | (kc)) */
// /* #define RSA(kc) (QK_RSFT | QK_RALT | (kc)) */
// /* #define RCS(kc) (QK_RCTL | QK_RSFT | (kc)) */
// /* #define SAGR(kc) RSA(kc) */

// #include "bluetooth.hpp"
class Keyboard {
  /**
   * @brief      Main keyboard class
   *
   * @details Binds together  all the necessary ingredients.
   * This includes holding the  mesh for esp-now, bluetooth,
   * and matrix  related function. This should  be the class
   * that a user interfaces with.
   */
public:
  Config *config;

  Matrix *matrix;
  Mesh *mesh;
  Display *display;
  LED *led;
  BleKeyboard bluetooth;
  EventManager *manager;

  RotaryEncoder *rotary_encoder;

  Keyboard(Config *config);
  ~Keyboard();
  void begin();
  void update();
  // communicate with bluetooth
  void process_keyswitches();
  void process_keyswitch(keyswitch_t &keyswitch, bool add_special);

  // special keycode functions
  void process_special_keycodes();
  void keep_active(keyswitch_t &keyswitch);

  bool is_server;
  double get_battery_level();

  layers_t layers;
  void sleep();
  void wakeup();

private:
  layer_t *active_layer;
  uint8_t active_layer_num;
  size_t last_activity;
  std::unordered_map<std::string,
                     std::unordered_map<std::string, const MediaKeyReport *>>
      encoder_codes;
  std::vector<keyswitch_t> special_keycodes;

  size_t get_last_activity();
  void set_active_layer(uint8_t layer);
};
#endif
