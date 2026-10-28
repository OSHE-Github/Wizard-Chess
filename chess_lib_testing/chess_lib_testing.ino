#include "chess.h"

#define LED_OK 2
#define LED_ILLEGAL 3
#define LED_WHITE 4
#define LED_BLACK 5
#define BTN_NEXT 6

chess_game_t chess_game;

typedef struct c_move {
  uint32_t FROM;
  uint32_t TO;
} c_move;

// A list of moves with some illegal ones to simulate board input
struct c_move get_next_move() {
  static uint32_t move_number = 0;
  c_move next_move;
  switch (move_number) {
    case 0:
      next_move = {11, 27};
      break;
    case 1:
      next_move = {52, 36};
      break;
    case 2:
      next_move = {27, 36};
      break;
    case 3:
      next_move = {57, 42};
      break;
    case 4:
      next_move = {42, 25};
      break;
    case 5:
      next_move = {6, 21};
      break;
    case 6:
      next_move = {61, 47};
      break;
    case 7:
      next_move = {49, 41};
      break;
    default:
    while(1){}
  }
  move_number++;
  return next_move;
}

void wait_next() {
  while (digitalRead(BTN_NEXT)) {}
  while (!digitalRead(BTN_NEXT)) {}
}

void updateTurn() {
  int team = chess_turn(&chess_game);
  uint64_t hey = 0;
  digitalWrite(LED_BLACK, team);
  digitalWrite(LED_WHITE, !team);
  Serial.println(team);
}

void setup() {
  Serial.begin(9600);

  // pinMode(LED_OK, OUTPUT);
  // pinMode(LED_ILLEGAL, OUTPUT);
  // pinMode(LED_WHITE, OUTPUT);
  // pinMode(LED_BLACK, OUTPUT);
  // pinMode(BTN_NEXT, INPUT_PULLUP);

  chess_init(&chess_game);
}

void loop() {
  updateTurn();
  delay(100);
  Serial.println("turn updated & wait next");
  // wait_next();

  // Make the move
  c_move next_move = get_next_move();
  int32_t move_result = chess_move(&chess_game, next_move.FROM, next_move.TO);

  Serial.println(move_result);

  // move status update 
  if (move_result == -2) {
    digitalWrite(LED_OK, LOW);
    digitalWrite(LED_ILLEGAL, HIGH);
    Serial.println("Illegal Move");
  }
  else {
    digitalWrite(LED_OK, HIGH);
    digitalWrite(LED_ILLEGAL, LOW);
    Serial.println("Move OK");
  }

  Serial.println("Updated status");
}
