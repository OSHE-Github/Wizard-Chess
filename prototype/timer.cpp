#include "timer.h"
#include "display.h"
#include <Arduino.h>

extern Display *display;

// Constructor
Timer::Timer() {
  // start timers at specified time, default is 10 minutes
  white_time = 0;
  black_time = 0;
  increment = 0;

  // Get time of last update for conversion
  last_update = millis();

  // White starts first
  white_turn = true;
  running = false;
  configured = false;
}

void Timer::start() {
  // Prevent restarting game before reset
  if (white_time == 0 || black_time == 0) return;

  // Start clock
  last_update = millis();
  running = true;
}

void Timer::pause(){
  update();
  running = false;
}

void Timer::end(){
  running = false;
}

void Timer::reset(uint32_t initial_time, uint32_t turnIncrement) {

  // reset timer at specified time, default is 10 minutes
  white_time = initial_time * 60000;
  black_time = initial_time * 60000;
  // Set turn increment in seconds

  increment = turnIncrement * 1000;

  // Get time of last update for conversion
  last_update = millis();

  // White starts first
  white_turn = true;
  running = false;
}

void Timer::update() {
  // Exit if timer isn't running
  if (!running) return;

  uint32_t current_time = millis();
  uint32_t elapsed_time = current_time - last_update;

  if (white_turn) {
    if (elapsed_time >= white_time) 
      white_time = 0;
    else 
      white_time -= elapsed_time;
    
  } 
  else {
    if (elapsed_time >= black_time)
      black_time = 0;
    else
      black_time -= elapsed_time;
  }

  last_update = current_time;

  if (white_time == 0 || black_time == 0) {
    running = false;
    display->set_screen(GAME_OVER);
  }
}

void Timer::swapTurn() {
  update();

  // Flip boolean value of who's turn it is
  if (running) {
    if (white_turn)
      white_time += increment;
    else 
      black_time += increment;
   
    white_turn = !white_turn;
  }
}

uint32_t Timer::get_white_time() {
  return white_time;
}

uint32_t Timer::get_black_time() {
  return black_time;
}

bool Timer::is_running() {
  return running;
}

bool Timer::is_white_turn() {
  return white_turn;
}

// Return 1 if white runs out of time, 2 if black does
uint8_t Timer::is_time_up() {
  if (white_time == 0)
    return 1;
  else if (black_time == 0)
    return 2;
  else
    return 0;
}

bool Timer::is_configured() {
  return configured;
}

void Timer::set_configured() {
  configured = 1;
}