
#include <OLED_I2C.h>
#include "display.h"

#define PRINT_DEBUG 0 // 1 = on 0 = off

// Sensor Array 
#define SHIFT_REG_SHLD 22
#define SHIFT_REG_CLOCK 23
#define SHIFT_REG_INPUT 25

#define SHIFT_REG_READ_DELAY 1 // microseconds

// Displays
Display *display;

// Buttons
#define BTN_LEFT_PIN 12
#define BTN_SELECT_PIN 
#define BTN_RIGHT_PIN 14
#define BTN_WHITE_TIMER_PIN
#define BTN_BLACK_TIMER_PIN

// Button Masks
#define BTN_LEFT 0b1
#define BTN_SELECT 0b10
#define BTN_RIGHT 0b100
#define BTN_WHITE_TIMER 0b1000
#define BTN_BLACK_TIMER 0b10000

volatile uint8_t buttons;

void Btn_Left_ISR();
void Btn_Select_ISR();
void Btn_Right_ISR();
void Btn_White_Timer_ISR();
void Btn_Black_Timer_ISR();

// Globals

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

  // Serial.print(buttons, BIN);
  // Serial.print("    ");
  // Serial.println(last_read);

  // temp disply test code
  if ((buttons & BTN_LEFT)) {
    last_read--;
    display->set_debug_data(last_read);
    buttons -= BTN_LEFT;
  }
  if ((buttons & BTN_RIGHT)) {
    last_read++;
    display->set_debug_data(last_read);
    buttons -= BTN_RIGHT;
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
  pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_RIGHT_PIN), Btn_Right_ISR, RISING);
  // pinMode(BTN_WHITE_TIMER_PIN, INPUT_PULLUP);
  // pinMode(BTN_BACK_TIMER_PIN, INPUT_PULLUP);

  Serial.println("SETUP COMPLETE");
}

void loop() 
{
  // read and update the board
  update_board();


  // update timer values

  // update displays
  display->update();
}

// ISR
void Btn_Left_ISR() 
{
  buttons |= BTN_LEFT;
}
void Btn_Select_ISR()
{
  buttons |= BTN_SELECT;
}
void Btn_Right_ISR()
{
  buttons |= BTN_RIGHT;
}
void Btn_White_Timer_ISR()
{
  buttons |= BTN_WHITE_TIMER;
}
void Btn_Black_Timer_ISR()
{
  buttons |= BTN_BLACK_TIMER;
}
