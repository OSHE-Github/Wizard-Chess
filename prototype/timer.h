#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

class Timer {
  private:

  // keep track of current timers
  uint32_t white_time;
  uint32_t black_time;

  // previous time
  uint32_t last_update;

  // whos on the clock
  bool white_turn;

  // is timer on?
  bool running;

  public:
  // Constructor
  Timer(uint32_t start_time_min = 10);

  // timer management
  void start();
  void pause();
  void reset(uint32_t initial_time = 10);

  // game stuff
  void update();
  void swapTurn();

  // getters!
  uint32_t get_white_time();
  uint32_t get_black_time();
  bool is_running();
  bool is_white_turn();
  bool is_game_over();

};


#endif