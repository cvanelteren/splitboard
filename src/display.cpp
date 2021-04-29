#include "display.hpp"
Display::Display(Config *config)
    : U8G2_SSD1306_128X64_NONAME_1_SW_I2C(config->display_rotation,
                                          config->pin_scl, config->pin_sda,
                                          config->pin_reset) {

  // this->u8g2 = U8G2_SSD1306_128X64_NONAME_1_SW_I2C(
  //     config->display_rotation, config->pin_scl, config->pin_sda,
  //     config->pin_reset);
}

// void Display::print(uint8_t x, uint8_t y, char text) {
//  u8g2_uint_t x;
//  uint8_t offset = 0;
//  auto width = this->u8g2->getUTF8Width(text);
//  u8g2.firstPage();
//  do {
//    // draw the scrolling text at current offset
//    x = offset;
//    this->u8g2.setFont(font);           // set the target font
//    do {                                // repeated drawing of the scrolling
//      this->u8g2.drawUTF8(x, 30, text); // draw the scolling text
//      x += width;                       // add the pixel width of the
//      scrolling text
//    } while (x < this->u8g2.getDisplayWidth()); // draw again until the
//    complete
//                                                // display is filled
//
//    this->u8g2.setFont(font); // draw the current pixel width
//    this->u8g2.setCursor(0, 58);
//    this->u8g2.print(
//        width); // this value must be lesser than 128 unless U8G2_16BIT is
//    set
//
//  } while (this->u8g2.nextPage());
//  offset -= 5; // scroll by one pixel
//  if ((u8g2_uint_t)offset < (u8g2_uint_t)-width)
//    offset = 0; // start over again
//}
