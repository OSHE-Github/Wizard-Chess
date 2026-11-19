#ifndef DISPLAY_H
#define DISPLAY_H

#include <OLED_I2C.h>
#include "timer.h"
#include "chess.h"

// Button Masks
#define BTN_LEFT (1 << 0)
#define BTN_SELECT (1 << 1)
#define BTN_RIGHT (1 << 2)
#define BTN_WHITE_TIMER (1 << 3)
#define BTN_BLACK_TIMER (1 << 4)

enum Screen {
  TIMER_CONF,
  BOARD_DISPLAY,
  TIMER_DISPLAY,
  PROMOTION,
  GAME_OVER,
  SPLASH,
  DEBUG
};

// Custom OLED class to add support for multiple displays
class mOLED : public OLED {
  public:
		mOLED(uint8_t data_pin, uint8_t sclk_pin, uint8_t rst_pin);
		mOLED(uint8_t data_pin, uint8_t sclk_pin);
    void update();  // if this doesnt work right, indicate this overrides the base class update function
    void invert(bool mode);
    uint8_t* _getScreen();
  private:
};

class Display {
protected:
  mOLED *white1;// = OLED(WHITE_SCR_1_SDA, WHITE_SCR_1_SCL);
  mOLED *white2;// = OLED(WHITE_SCR_2_SDA, WHITE_SCR_2_SCL);
  mOLED *black1;// = OLED(BLACK_SCR_1_SDA, BLACK_SCR_1_SCL);
  mOLED *black2;// = OLED(BLACK_SCR_2_SDA, BLACK_SCR_2_SCL);
  Screen screen;
  Timer *timer;
  chess_game_t *chess_game;
  uint64_t debug_data[2];
  bool refresh, invalid_move;


  void timer_configuration();
  void board_display();
  void timer_display();
  void promotion();
  void game_over();
  void splash_screen();
  void debug_screen();
  void draw_invalid();

public:
  Display();
  void update();
  void set_screen(Screen);
  void set_timer(Timer *t);
  void set_invalid_mode(bool);
  void set_debug_data(uint32_t, uint32_t);
  void set_chess_board(chess_game_t*);
};

#endif