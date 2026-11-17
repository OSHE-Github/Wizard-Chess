#include "timer.h"
#include <Arduino.h>

// Constructor
Timer::Timer(uint32_t start_time_min) {
  // start timers at specified time, default is 10 minutes
  white_time = start_time_min * 60000;
  black_time = start_time_min * 60000;

  // Get time of last update for conversion
  last_update = millis();

  // White starts first
  white_turn = true;
  running = false;
}

void Timer::start() {
  // Get new time and start clock
  last_update = millis();
  running = true;
}

void Timer::pause(){
  update();
  running = false;
}

void Timer::reset(uint32_t initial_time) {
  // reset timer at specified time, default is 10 minutes
  white_time = initial_time * 60000;
  black_time = initial_time * 60000;

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

  if (white_time == 0 || black_time == 0)
    running = false;

}

void Timer::swapTurn() {
  update();

  // Flip boolean value of who's turn it is
  if (running) 
    white_turn = !white_turn;

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