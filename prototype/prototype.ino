
#include "display.h"

#define PRINT_DEBUG 1 // 1 = on 0 = off

// Sensor Array 
#define SHIFT_REG_SHLD 22
#define SHIFT_REG_CLOCK 23
#define SHIFT_REG_INPUT 25

#define SHIFT_REG_READ_DELAY 1 // microseconds

// Displays

// Buttons

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

  // Buttons

  Serial.println("SETUP COMPLETE");
}

void loop() 
{
  // Read the board

  // Check if a move was made

  // if so, validate move & report

  // update timer values

  // update displays

}
