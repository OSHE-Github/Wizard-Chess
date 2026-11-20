#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

class Timer {
  private:

  // keep track of current timers
  uint32_t white_time;
  uint32_t black_time;

  uint32_t increment;
  // previous time
  uint32_t last_update;
  // whos on the clock
  bool white_turn;
  // is timer on?
  bool running;
  bool configured;

  public:
  // Constructor
  Timer();

  // timer management
  void start();
  void pause();
  void end();
  void reset(uint32_t initial_time, uint32_t turnIncrement);

  // game stuff
  void update();
  void swapTurn();

  // getters!
  uint32_t get_white_time();
  uint32_t get_black_time();
  bool is_running();
  bool is_white_turn();
  uint8_t is_time_up();
  bool is_configured();

  // setter
  void set_configured();

};


#endif