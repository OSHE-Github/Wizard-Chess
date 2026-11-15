#ifndef DISPLAY_H
#define DISPLAY_H

#include <OLED_I2C.h>

enum Screen {
  TIMER_CONF,
  BOARD_DISPLAY,
  TIMER_DISPLAY,
  SPLASH,
  DEBUG
};

class Display {
private:
  OLED *white1;// = OLED(WHITE_SCR_1_SDA, WHITE_SCR_1_SCL);
  OLED *white2;// = OLED(WHITE_SCR_2_SDA, WHITE_SCR_2_SCL);
  OLED *black1;// = OLED(BLACK_SCR_1_SDA, BLACK_SCR_1_SCL);
  OLED *black2;// = OLED(BLACK_SCR_2_SDA, BLACK_SCR_2_SCL);
  Screen screen;
  uint64_t debug_data;
  bool refresh, invaid_move;

  void timer_configuration();
  void board_display();
  void timer_display();
  void splash_screen();
  void debug_screen();

public:
  Display();
  void update();
  void set_screen(Screen);
  void set_invalid_mode(bool);
  void set_debug_data(uint64_t);
};

#endif