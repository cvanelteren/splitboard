#include "keymap.hpp"

/* uint8_t layer_1[config::row_pins.size()][config::col_pins.size()] */

// KC = keycode
using namespace key_layers;
const layer_t key_layers::qwerty = {
    {KC_TILDE, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0,
     KC_BSPC},
    {KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P,
     KC_OBRACKET, KC_CBRACKET},
    {KC_TAB, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_COLON,
     KC_SQUOTE},
    {KC_LSHIFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMMA, KC_DOT,
     KC_SLASH, KC_RSHIFT},
    {KC_NO, KC_NO, KC_NO, KC_LCTL, KC_ALT, KC_SPC, KC_ENTER, KC_SUPER, KC_SUPER,
     KC_NO, KC_NO, KC_NO}};
