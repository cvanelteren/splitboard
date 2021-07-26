// // controls
// #define KC_BACPSPACE 0x2a // BS	Backspace
// #define KC_TAB 0x2b       // TAB	Tab
// #define KC_ENTER 0x28     // LF	Enter

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
#define KC_M 0x6d
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

// capitals

#define SHIFT 0x80

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
// #define KC_EQ 0x2e           // =
// #define KC_MINUS 0x2d        // -
// #define KC_PLUS 0x2e | SHIFT // +
// #define KC_BSP

// // symbols
// #define KC_COLON 0x3A    // :
// #define KC_SCOLON 0x3B   // ;
// #define KC_LT 0x3C       // <
// #define KC_GT 0x3D       // >
// #define KC_QUESTION 0x3F // ?
// // #define KC_AT 40         // @o

// #define KC_SLASH 0x2F    // /
// #define KC_OBRACKET 0x5B // [
// #define KC_BSLASH 0x5C   // bslash
// #define KC_CBRACKET 0x5D // ]

// #define KC_HAT 0x5E               //^
// #define KC_US 0x2d | SHIFT        // _
// #define KC_BACKTICK 0x35          // `
// #define KC_LCBACKET 0x2f | SHIFT  // {
// #define KC_VMID 0x31 | SHIFT      // |
// #define KC_CCBRACKET 0x30 | SHIFT // }
// #define KC_TILDE 0x35 | SHIFT     // ~
// #define KC_DEL 0                  // DEL
// #define KC_SPC 0x2c               //  ' '
// #define KC_EXCLAIM 0x1e | SHIFT   // !
// #define KC_QUOTE 0x34 | SHIFT     // "
// #define KC_HASH 0x20 | SHIFT      // #
// #define KC_DOLLAR 0x21 | SHIFT    // $
// #define KC_PRCT 0x22 | SHIFT      // %
// #define KC_AND 0x24 | SHIFT       // &
// #define KC_SQUOTE 0x34            // '
// #define KC_OPAREN 0x26 | SHIFT    // (
// #define KC_CPAREN 0x27 | SHIFT    // )
// #define KC_ASTERISK 0x25 | SHIFT  // *
// #define KC_PLUS 0x2e | SHIFT      // +
// #define KC_COMMA 0x36             // ,
// #define KC_DASH 0x2d              // -
// #define KC_DOT 0x37               // .
// #define KC_AT 0x1f | SHIFT        // @

// #define KC_NUL 0x00 // NUL
// #define KC_SOH 0x00 // SOH
// #define KC_STX 0x00 // STX
// #define KC_ETX 0x00 // ETX
// #define KC_EOT 0x00 // EOT
// #define KC_ENQ 0x00 // ENQ
// #define KC_ACK 0x00 // ACK
// #define KC_BEL 0x00 // BEL
// #define KC_VT 0x00  // VT
// #define KC_FF 0x00  // FF
// #define KC_CR 0x00  // CR
// #define KC_SO 0x00  // SO
// #define KC_SI 0x00  // SI
// #define KC_DEL 0    // DEL
// #define KC_DC1 0x00 // DC1
// #define KC_DC2 0x00 // DC2
// #define KC_DC3 0x00 // DC3
// #define KC_DC4 0x00 // DC4
// #define KC_NAK 0x00 // NAK
// #define KC_SYN 0x00 // SYN
// #define KC_ETB 0x00 // ETB
// #define KC_CAN 0x00 // CAN
// #define KC_EM 0x00  // EM
// #define KC_SUB 0x00 // SUB
// #define KC_ESC 0x00 // ESC
// #define KC_FS 0x00  // FS
// #define KC_GS 0x00  // GS
// #define KC_RS 0x00  // RS
// // #define KC_US 0x00  // US

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
