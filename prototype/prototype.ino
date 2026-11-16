
#include <OLED_I2C.h>
#include "display.h"

#define PRINT_DEBUG 0 // 1 = on 0 = off

// Sensor Array 
#define SHIFT_REG_SHLD 32
#define SHIFT_REG_CLOCK 35
#define SHIFT_REG_INPUT 34

#define SHIFT_REG_READ_DELAY 1 // microseconds

// Displays (See display.cpp for pin assignments)
Display *display;

// Button Pins
#define BTN_LEFT_PIN 12
#define BTN_SELECT_PIN 
#define BTN_RIGHT_PIN 14
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

  uint64_t value = shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST);
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 8;
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 16;
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 24;
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 32;
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 40;
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 48;
  value |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, MSBFIRST) << 56;
  if (PRINT_DEBUG) {
    for (int i = 0; i < 8; i++) {
      Serial.print((uint8_t)(value >> (8*i)), BIN);
    }
    Serial.println();
  }

  Serial.print(button_itr, BIN);
  Serial.print("    ");
  Serial.print(button_states, BIN);
  Serial.print("    ");
  Serial.println(last_read);

  // temp disply test code
  if ((button_states & BTN_LEFT)) {
    last_read--;
    display->set_debug_data(last_read);
    button_states -= BTN_LEFT;
  }
  if ((button_states & BTN_RIGHT)) {
    last_read++;
    display->set_debug_data(last_read);
    button_states -= BTN_RIGHT;
  }

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
  
  // Buttons & Interupts
  Serial.println("Setting Interrupts");
  pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_LEFT_PIN), Btn_Left_ISR, RISING);
  // pinMode(BTN_SELECT_PIN, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(BTN_SELECT_PIN), Btn_Select_ISR, RISING);
  pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_RIGHT_PIN), Btn_Right_ISR, RISING);
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
