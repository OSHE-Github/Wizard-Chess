
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
#define BTN_LEFT_PIN 13
#define BTN_SELECT_PIN 32
#define BTN_RIGHT_PIN 5
#define BTN_WHITE_TIMER_PIN 4
#define BTN_BLACK_TIMER_PIN 21

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

enum BoardState {
  START,
  PEICE_SELECTED,
  MOVE_MADE,
  MOVE_VALIDATION,
  RESET_BOARD
};

// detect peice placed

int32_t test_peice_placed(uint32_t before[], uint32_t after[]) {
  int32_t changed_index = -1;
  int32_t change_count = 0;
  
  uint32_t data_before = before[0];
  uint32_t data_after = after[0]; 
  for (int i = 0; i < 32; i++) {
    if ((data_before & 1) && !(data_after & 1)) {
      changed_index = 63 - i;
      change_count++;
    }
    data_before = data_before >> 1;
    data_after = data_after >> 1;
  }

  data_before = before[1];
  data_after = after[1]; 
  for (int i = 0; i < 32; i++) {
    if ((data_before & 1) && !(data_after & 1)) {
      changed_index = 63 - (i + 32);
      change_count++;
    }
    data_before = data_before >> 1;
    data_after = data_after >> 1;
  }

  if (change_count > 1) {
    changed_index = -2;
  }

  return changed_index;
}

// detect peice picked
int32_t test_peice_picked(uint32_t before[], uint32_t after[]) {
  int32_t changed_index = -1;
  int32_t change_count = 0;
  
  uint32_t data_before = before[0];
  uint32_t data_after = after[0]; 
  for (int i = 0; i < 32; i++) {
    if (!(data_before & 1) && (data_after & 1)) {
      changed_index = 63 - i;
      change_count++;
    }
    data_before = data_before >> 1;
    data_after = data_after >> 1;
  }

  data_before = before[1];
  data_after = after[1]; 
  for (int i = 0; i < 32; i++) {
    if (!(data_before & 1) && (data_after & 1)) {
      changed_index = 63 - (i + 32);
      change_count++;
    }
    data_before = data_before >> 1;
    data_after = data_after >> 1;
  }

  if (change_count > 1) {
    changed_index = -2;
  }

  return changed_index;
}

bool update_board() 
{
  static uint32_t last_read[] = {0, 0};
  static uint32_t last_valid_read[] = {0, 0};
  static unsigned long last_board_change = 0; 
  static int32_t to = -1;
  static int32_t from = -1;
  static BoardState state = START;

  // Read Current Board
  digitalWrite(SHIFT_REG_SHLD, HIGH);
  delayMicroseconds(SHIFT_REG_READ_DELAY);
  digitalWrite(SHIFT_REG_SHLD, LOW);
  delayMicroseconds(SHIFT_REG_READ_DELAY);
  digitalWrite(SHIFT_REG_SHLD, HIGH);
  delayMicroseconds(SHIFT_REG_READ_DELAY);

  uint32_t new_read[] = {0, 0};
  uint8_t bitorder = LSBFIRST;
  new_read[0] |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder);
  new_read[0] |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder) << 8;
  new_read[0] |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder) << 16;
  new_read[0] |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder) << 24;
  new_read[1] |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder);
  new_read[1] |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder) << 8;
  new_read[1] |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder) << 16;
  new_read[1] |= shiftIn(SHIFT_REG_INPUT, SHIFT_REG_CLOCK, bitorder) << 24;

  // if this is the first board read set last state and skip the rest of this iter
  if (last_read[0] == 0 && last_read[1] == 0) {
    last_read[0] = new_read[0];
    last_read[1] = new_read[1];
    last_valid_read[0] = new_read[0];
    last_valid_read[1] = new_read[1];
    return false;
  }

  if (PRINT_DEBUG) {
    // for (int i = 0; i < 8; i++) {
    //   Serial.print((uint8_t)(value >> (8*i)), BIN);
    // }
    // Serial.println();
    Serial.print(new_read[0], BIN);
    Serial.print(" ");
    Serial.println(new_read[1], BIN);
  }

  display->set_debug_data(new_read[0], new_read[1]);

  // Start State Machine
  
  int32_t changed_index = 0;
  uint8_t button_mask = 0;
  uint32_t move_result = 0;

  // Serial.print(test_peice_picked(last_read, new_read));
  // Serial.print(test_peice_placed(last_read, new_read));

  Serial.print(from);
  Serial.print(" ");
  Serial.println(to);

  // state = (BoardState)20;
  switch (state) {
    case START:
      Serial.println("State Start");
       changed_index = test_peice_picked(last_valid_read, new_read);
       if (changed_index >= 0) {
        from = changed_index;
        state = PEICE_SELECTED;
       }
      //  else if (changed_index < -1) {
      //   state = RESET_BOARD;
      //  }
    break;
    case PEICE_SELECTED:
    Serial.println("State Peice Selected");
       changed_index = test_peice_placed(last_read, new_read);
       if (changed_index >= 0) {
        if (changed_index == from) {
          from = -1;
          state = START;
          break;
        }
        to = changed_index;
        state = MOVE_MADE;
        button_states -= BTN_WHITE_TIMER;
        button_states -= BTN_BLACK_TIMER;
       }
    break;
    case MOVE_MADE:
      Serial.println("State Move Made");
       changed_index = test_peice_picked(last_read, new_read);
       if (changed_index >= 0 && changed_index == to) {
        to = -1;
        state = PEICE_SELECTED;
       }
      if (chess_game.turn == CHESS_WHITE) {
        button_mask = BTN_WHITE_TIMER;
      }
      else {
        button_mask = BTN_BLACK_TIMER;
      }
      if ((button_states & button_mask)) {
        state = MOVE_VALIDATION;
        button_states -= button_mask;
      }
    break;
    case MOVE_VALIDATION:
    Serial.println("State Validation");
    move_result = chess_move(&chess_game, from, to);
    if (move_result == -2) {
      state = RESET_BOARD;
      display->set_invalid_mode(true);
      break;
    }
    state = START;
    last_valid_read[0] = new_read[0];
    last_valid_read[1] = new_read[1];
    to = -1;
    from = -1;
    timer->swapTurn();
    break;
    case RESET_BOARD:
    Serial.println("State RESET");
    if (new_read[0] == last_valid_read[0] && new_read[1] == last_valid_read[1]) {
      display->set_invalid_mode(false);
      state = START;
      to = -1;
      from = -1;
    }
    break;
    default:
    Serial.println("INVALID BOARD STATE");
  }

  last_read[0] = new_read[0];
  last_read[1] = new_read[1];

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

  // Chess Engine
  chess_init(&chess_game);
  display->set_chess_board(&chess_game);
  
  // Buttons & Interupts
  Serial.println("Setting Interrupts");
  pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_LEFT_PIN), Btn_Left_ISR, FALLING);
  pinMode(BTN_SELECT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_SELECT_PIN), Btn_Select_ISR, FALLING);
  pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_RIGHT_PIN), Btn_Right_ISR, FALLING);
  pinMode(BTN_WHITE_TIMER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_WHITE_TIMER_PIN), Btn_White_Timer_ISR, FALLING);
  pinMode(BTN_BLACK_TIMER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_BLACK_TIMER_PIN), Btn_Black_Timer_ISR, FALLING);

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
    bool white = timer->is_white_turn();
    if (((button_states & BTN_WHITE_TIMER) && white) || (!white && (button_states & BTN_BLACK_TIMER))) {
      timer->swapTurn();
      button_states = 0;
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
