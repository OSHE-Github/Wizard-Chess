#ifndef DISPLAY_H
#define DISPLAY_H

#define WHITE_SCR_1_SDA
#define WHITE_SCR_1_SCL
#define WHITE_SCR_2_SDA
#define WHITE_SCR_2_SCL

#define BLACK_SCR_1_SDA
#define BLACK_SCR_1_SCL
#define BLACK_SCR_2_SDA
#define BLACK_SCR_2_SCL

enum Screen {
  TIMER_CONF,
  BOARD_DISPLAY,
  TIMER_DISPLAY,
  SPLASH,
  DEBUG
};

class Display {
private:
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
};

#endif