
#include <OLED_I2C.h>
#include "display.h"
#include "timer.h"

#define PRINT_DEBUG 0 // 1 = on 0 = off
#define TIMER_TEST 0


// Sensor Array 
#define SHIFT_REG_SHLD 12
#define SHIFT_REG_CLOCK 14
#define SHIFT_REG_INPUT 34

#define SHIFT_REG_READ_DELAY 1 // microseconds

// Displays (See display.cpp for pin assignments)
Display *display;
Timer *timer;
chess_game_t chess_game;

// Button Pins
#define BTN_LEFT_PIN 
#define BTN_SELECT_PIN 
#define BTN_RIGHT_PIN 
#define BTN_WHITE_TIMER_PIN
#define BTN_BLACK_TIMER_PIN

// Button Masks
#define BTN_LEFT (1 << 0)
#define BTN_SELECT (1 << 1)
#define BTN_RIGHT (1 << 2)
#define BTN_WHITE_TIMER (1 << 3)
#define BTN_BLACK_TIMER (1 << 4)

// Button Interrupts
volatile uint8_t button_itr;
uint8_t button_states;
unsigned long button_debounce_times[7];

#define BTN_DEBOUNCE 200 // milliseconds

void Btn_Left_ISR();
void Btn_Select_ISR();
void Btn_Right_ISR();
void Btn_White_Timer_ISR();
void Btn_Black_Timer_ISR();

// Globals

void update_buttons()
{
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t mask = (1 << i);
    if (button_itr & mask) {
      if ((millis() - button_debounce_times[i]) > BTN_DEBOUNCE) {
        button_states |= mask;
        button_debounce_times[i] = millis();
      }
      button_itr -= mask;
    }
  }
}

unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

bool update_board() 
{
  uint64_t current_read = 0;
  static uint64_t last_read = 0; // default setup: ((0b1111111111111111 << 48) | 0b1111111111111111);
  static unsigned long last_board_change = 0; 
  static int32_t to = -1;
  static int32_t from = -1;

  // Read Current Board
  digitalWrite(SHIFT_REG_SHLD, HIGH);
  delayMicroseconds(SHIFT_REG_READ_DELAY);
  digitalWrite(SHIFT_REG_SHLD, LOW);
  delayMicroseconds(SHIFT_REG_READ_DELAY);
  digitalWrite(SHIFT_REG_SHLD, HIGH);
  delayMicroseconds(SHIFT_REG_READ_DELAY);

  uint32_t new_read_low = 0;
  uint32_t new_read_high = 0;
  uint8_t bitorder = LSBFIRST;
  new_read_low |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder);
  new_read_low |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder) << 8;
  new_read_low |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder) << 16;
  new_read_low |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder) << 24;
  new_read_high |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder);
  new_read_high |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder) << 8;
  new_read_high |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder) << 16;
  new_read_high |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder) << 24;

  if (PRINT_DEBUG) {
    // for (int i = 0; i < 8; i++) {
    //   Serial.print((uint8_t)(value >> (8*i)), BIN);
    // }
    // Serial.println();
    Serial.print(new_read_low, BIN);
    Serial.print(" ");
    Serial.println(new_read_high, BIN);
  }

  display->set_debug_data(new_read_low, new_read_high);

  return false;
}

void setup() 
{
  Serial.begin(115200);
  

  // Sensor Array
  pinMode(SHIFT_REG_CLOCK, OUTPUT);
  pinMode(SHIFT_REG_SHLD, OUTPUT);
  pinMode(SHIFT_REG_INPUT, INPUT);

  // Displays
  Serial.println("Starting Displays");
  display = new Display();

  // Timer
  Serial.println("Initializing timer");
  timer = new Timer();
  // Pass timer to display
  display->set_timer(timer);

  #if TIMER_TEST
    // 1 minute test clock
    timer->reset(1);
    timer->start();

    display->set_screen(TIMER_DISPLAY);
  #endif

  // Chess Engine
  chess_init(&chess_game);
  display->set_chess_board(&chess_game);
  
  // Buttons & Interupts
  Serial.println("Setting Interrupts");
  // pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(BTN_LEFT_PIN), Btn_Left_ISR, RISING);
  // pinMode(BTN_SELECT_PIN, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(BTN_SELECT_PIN), Btn_Select_ISR, RISING);
  // pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(BTN_RIGHT_PIN), Btn_Right_ISR, RISING);
  // pinMode(BTN_WHITE_TIMER_PIN, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(BTN_WHITE_TIMER_PIN), Btn_White_Timer_ISR, RISING);
  // pinMode(BTN_BACK_TIMER_PIN, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(BTN_LEFT_PIN), Btn_Black_Timer_ISR, RISING);
  button_itr = 0;
  button_states = 0;
  memset(button_debounce_times, 0, 8*sizeof(unsigned long));

  Serial.println("SETUP COMPLETE");
}

void loop() 
{
  // update buttons
  update_buttons();
  // read and update the board
  update_board();

  // update timer values
  timer->update();

  
  

  #if TIMER_TEST
    static uint32_t test_start = millis();

    if (millis() - test_start >= 10000) {
        timer->swapTurn();
        test_start += 10000;

        Serial.println("Switched timer");
    }
  #endif 


  // update displays
  display->update();
  // delay(1000);
}

// ISR
void Btn_Left_ISR() 
{
  button_itr |= BTN_LEFT;
}
void Btn_Select_ISR()
{
  button_itr |= BTN_SELECT;
}
void Btn_Right_ISR()
{
  button_itr |= BTN_RIGHT;
}
void Btn_White_Timer_ISR()
{
  button_itr |= BTN_WHITE_TIMER;
}
void Btn_Black_Timer_ISR()
{
  button_itr |= BTN_BLACK_TIMER;
}
