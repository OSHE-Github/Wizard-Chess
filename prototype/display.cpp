#include <OLED_I2C.h>
#include <Wire.h>
#include <cstdio>
#include "display.h"
#include "bitmaps.h"


#define WHITE_SCR_1_SDA 18
#define WHITE_SCR_1_SCL 19
#define WHITE_SCR_2_SDA 22
#define WHITE_SCR_2_SCL 23

#define BLACK_SCR_1_SDA 27
#define BLACK_SCR_1_SCL 26
#define BLACK_SCR_2_SDA 25
#define BLACK_SCR_2_SCL 33

#define SPLASH_BREAK 5000


extern uint8_t button_states;
extern bool game_start;
extern bool game_ready;
extern bool timed_game;
extern uint8_t BigFont[];
extern uint8_t SmallFont[];

//////// Private Functions

void Display::timer_configuration() {
  enum configPhase {
    FIRST,
    RAPID,
    FIVE,
    BLITZ,
    BULLET,
    FIFTEEN_TEN,
    THREE_TWO,
    NO_TIMER,
    SET,
    START
  };

  static uint32_t set_time = 0;
  static uint32_t set_increment = 0;

  static configPhase phase = FIRST;
  // Set rapid as default config 
  

  white1->setFont(BigFont);

  switch(phase) {
    
    case FIRST:
      white1->clrScr();
      white1->print("Set Time", CENTER, 12);
      white1->setFont(SmallFont);
      white1->print("Press Select", CENTER, 48);

      
      black1->clrScr();
      black1->setFont(SmallFont);
      black1->print("Waiting for white", CENTER, 20);
      black1->print("to setup time O_o", CENTER, 30);
      
      
      // Select pressed
      if (button_states & (1 << 1) )
        phase = RAPID;
  
      break;
    case RAPID:
      white1->clrScr();
      white1->print("RAPID", CENTER, 4);
      white1->setFont(SmallFont);
      white1->print("10 minutes", CENTER, 24);
      white1->print("<-   OK   ->", CENTER, 52);

      if (button_states & BTN_LEFT) 
        phase = NO_TIMER;
      else if (button_states & BTN_SELECT) {
        phase = SET;
        set_time = 10;
        set_increment = 0;
      } else if (button_states & BTN_RIGHT)
        phase = FIVE;

      break;
    case FIVE:
      white1->clrScr();
      white1->print("BLITZ 5", CENTER, 4);
      white1->setFont(SmallFont);
      white1->print("5 minutes", CENTER, 24);
      white1->print("<-   OK   ->", CENTER, 52);

      if (button_states & BTN_LEFT) 
        phase = RAPID;
      else if (button_states & BTN_SELECT) {
        phase = SET;
        set_time = 5;
        set_increment = 0;
      } else if (button_states & BTN_RIGHT)
        phase = BLITZ;

      break;
    
    case BLITZ:
      white1->clrScr();
      white1->print("BLITZ 3", CENTER, 4);
      white1->setFont(SmallFont);
      white1->print("3 minutes", CENTER, 24);
      white1->print("<-   OK   ->", CENTER, 52);

      if (button_states & BTN_LEFT) 
        phase = FIVE;
      else if (button_states & BTN_SELECT) {
        phase = SET;
        set_time = 3;
        set_increment = 0;
      } else if (button_states & BTN_RIGHT)
        phase = BULLET;

      
      break;
    case BULLET:
      white1->clrScr();
      white1->print("BULLET", CENTER, 4);
      white1->setFont(SmallFont);
      white1->print("1 minute", CENTER, 24);
      white1->print("<-   OK   ->", CENTER, 52);

      if (button_states & BTN_LEFT) 
        phase = BLITZ;
      else if (button_states & BTN_SELECT) {
        phase = SET;
        set_time = 1;
        set_increment = 0;
      } else if (button_states & BTN_RIGHT)
        phase = FIFTEEN_TEN;

      break;
    case FIFTEEN_TEN:
      white1->clrScr();
      white1->print("15|10", CENTER, 4);
      white1->setFont(SmallFont);
      white1->print("15 min + 10 sec", CENTER, 24);
      white1->print("<-   OK   ->", CENTER, 52);

      if (button_states & BTN_LEFT) 
        phase = BULLET;
      else if (button_states & BTN_SELECT) {
        phase = SET;
        set_time = 15;
        set_increment = 10;
      } else if (button_states & BTN_RIGHT)
        phase = THREE_TWO;

      break;
    case THREE_TWO:
      white1->clrScr();
      white1->print("3|2", CENTER, 4);
      white1->setFont(SmallFont);
      white1->print("3 min + 2 sec", CENTER, 24);
      white1->print("<-   OK   ->", CENTER, 52);

      if (button_states & BTN_LEFT) 
        phase = FIFTEEN_TEN;
      else if (button_states & BTN_SELECT) { 
        phase = SET;
        set_time = 3;
        set_increment = 2;
      } else if (button_states & BTN_RIGHT)
        phase = NO_TIMER;

      break;
    case NO_TIMER:
      white1->clrScr();
      white1->print("NO TIMER", CENTER, 4);
      white1->setFont(SmallFont);
      white1->print("Untimed & Auto move", CENTER, 24);
      white1->print("<-   OK   ->", CENTER, 52);

      if (button_states & BTN_LEFT) 
        phase = THREE_TWO;
      else if (button_states & BTN_SELECT) { 
        phase = SET;
        timed_game = false;
      } else if (button_states & BTN_RIGHT)
        phase = RAPID;

      break;
    case SET:
      // Pass timer settings to timer
      timer->reset(set_time, set_increment);
      timer->set_configured();

      white1->clrScr();
      white1->setFont(BigFont);
      white1->print("READY?", CENTER, 4);
      white1->setFont(SmallFont);
      white1->print("Black To Start", CENTER, 24);
      white1->print("<-          ", CENTER, 52);
      white2->clrScr();
      black1->clrScr();
      black1->setFont(BigFont);
      black1->print("READY?", CENTER, 4);
      black1->setFont(SmallFont);
      black1->print("Black To Start", CENTER, 24);
      black1->print("<-          ", CENTER, 52);
      black2->clrScr();

      game_start = true;

      if (game_ready) {
        if (button_states & BTN_LEFT) 
          phase = FIRST;
        else if (button_states & BTN_BLACK_TIMER || !timed_game){ 
          phase = START;
        } 
      }

      break;
      case START:
        // Transition to more traditional timer start system (i got sick of waiting 5s every time LOL)
        // static uint32_t start_start = millis();
        // static uint8_t count = 5;

        // if (millis() - start_start > 1000) {
        //   white1->clrScr();
        //   black1->clrScr();

        //   white1->setFont(SmallFont);
        //   black1->setFont(SmallFont);

        //   white1->print("Game starting...", CENTER, 8);
        //   white1->setFont(BigFont);
        //   white1->printNumI(count, CENTER, 32);

        //   black1->print("Game starting...", CENTER, 8);
        //   black1->setFont(BigFont);
        //   black1->printNumI(count, CENTER, 32);

        //   count -= 1;
        //   start_start += 1000;
        // }

      // if (count == 0) {
      // start timer (now handled by board state)
      if (timed_game) {
        timer->start();
        screen = TIMER_DISPLAY;
      }
      else {
        screen = UNTIMED_DISPLAY;
      }
      phase = FIRST;
      // game_start = true;
      // }
      break;
    default:
      break;
  }

  button_states = 0;
  refresh = true;

}

void Display::board_display()
{
  mOLED *scr = white2;
  for (int i = 0; i < 2; i++) {
    if (i == 1) {
      scr = black2;
    }
    scr->clrScr();
    scr->setFont(SmallFont);
    if (chess_game != NULL) {
      uint32_t data = debug_data[1];
      uint32_t offset = 1 << 31;
      uint8_t *tile_bitmap = NULL;
      bool inverted = false;
      for (int i = 0; i < 64; i++) {
        if (i == 32) {
          data = debug_data[0];
          offset = 1 << 31;
        }
        if (!((data) & offset)) {
          tile_bitmap = debug_tile;
          inverted = true;
        }
        else {
          tile_bitmap = empty_debug_tile;
          inverted = false;
        }
        offset = offset >> 1;
        
        chess_id_t current = chess_game->board[i];
        int x, y;
        // if (i == 1) {
        //   y = 7 - ((63 - i) / 8);
        //   x = ((63 - i) % 8);
        // }
        // else {
        y = ((63 - i) / 8);
        x = 7 - ((63 - i) % 8);
        // }
        scr->drawBitmap(x * 16, y * 8, tile_bitmap, 16, 8);
        switch (CHESS_TEAM(current)) {
          case CHESS_WHITE:
            switch (CHESS_TYPE(current)) {
              case CHESS_PAWN:
                scr->drawInvertedBitmap(x * 16, y * 8, wp, 16, 8, inverted);
                break;
              case CHESS_BISHOP:
                scr->drawInvertedBitmap(x * 16, y * 8, wb, 16, 8, inverted);
                break;
              case CHESS_ROOK:
                scr->drawInvertedBitmap(x * 16, y * 8, wr, 16, 8, inverted);
                break;
              case CHESS_KNIGHT:
                scr->drawInvertedBitmap(x * 16, y * 8, wh, 16, 8, inverted);
                break;
              case CHESS_QUEEN:
                scr->drawInvertedBitmap(x * 16, y * 8, wq, 16, 8, inverted);
                break;
              case CHESS_KING:
                scr->drawInvertedBitmap(x * 16, y * 8, wk, 16, 8, inverted);
                break;
              default:
                scr->drawBitmap(x * 16, y * 8, tile_bitmap, 16, 8);
            }
          break;
          case CHESS_BLACK:
            switch (CHESS_TYPE(current)) {
              case CHESS_PAWN:
                scr->drawInvertedBitmap(x * 16, y * 8, bp, 16, 8, inverted);
                break;
              case CHESS_BISHOP:
                scr->drawInvertedBitmap(x * 16, y * 8, bb, 16, 8, inverted);
                break;
              case CHESS_ROOK:
                scr->drawInvertedBitmap(x * 16, y * 8, br, 16, 8, inverted);
                break;
              case CHESS_KNIGHT:
                scr->drawInvertedBitmap(x * 16, y * 8, bh, 16, 8, inverted);
                break;
              case CHESS_QUEEN:
                scr->drawInvertedBitmap(x * 16, y * 8, bq, 16, 8, inverted);
                break;
              case CHESS_KING:
                scr->drawInvertedBitmap(x * 16, y * 8, bk, 16, 8, inverted);
                break;
              default:
                scr->drawBitmap(x * 16, y * 8, tile_bitmap, 16, 8);
            }
          break;
          default:
            scr->drawBitmap(x * 16, y * 8, tile_bitmap, 16, 8);
        }
      }
    }
  }
}

void Display::timer_display()
{
    if (timer == NULL)
        return;

    // if (white1 == NULL || black1 == NULL)
    //     return;
    
    static uint32_t last_screen_update = 0;
    static uint8_t last_btns = button_states;
    if (millis() - last_screen_update < 1000 && !refresh && !(last_btns ^ button_states)) {
      return;
    }

    
    last_screen_update = millis();
    last_btns = button_states;
    refresh = true;

    uint32_t white_time = timer->get_white_time();
    uint32_t black_time = timer->get_black_time();

    uint32_t white_total_seconds = (white_time + 999) / 1000;
    uint32_t black_total_seconds = (black_time + 999) / 1000;

    uint32_t white_minutes = white_total_seconds / 60;
    uint32_t white_seconds = white_total_seconds % 60;

    uint32_t black_minutes = black_total_seconds / 60;
    uint32_t black_seconds = black_total_seconds % 60;

    char white_buffer[6];
    char black_buffer[6];
    std::sprintf(white_buffer, "%d:%02d", white_minutes, white_seconds);
    std::sprintf(black_buffer, "%d:%02d", black_minutes, black_seconds);

    char white_score_buffer[10];
    char black_score_buffer[10];
    chess_score_t white_score = chess_score(chess_game, CHESS_WHITE);
    chess_score_t black_score = chess_score(chess_game, CHESS_BLACK);

    std::sprintf(white_score_buffer, "Score: %d", (int)white_score);
    std::sprintf(black_score_buffer, "Score: %d", (int)black_score);

    chess_status_t white_status;
    chess_status_t black_status;
    chess_status(chess_game, &white_status, &black_status);

    Serial.print(white_status);
    Serial.print(" ");
    Serial.println(black_status);

    white1->setFont(BigFont);
    black1->setFont(BigFont); 
    white1->clrScr();
    black1->clrScr();

    // Print timers in big font
    white1->print(white_buffer, CENTER, 8);
    black1->print(black_buffer, CENTER, 8);
    
    // Print score in small font
    white1->setFont(SmallFont);
    black1->setFont(SmallFont); 
    white1->print(white_score_buffer, RIGHT, 52);
    black1->print(black_score_buffer, RIGHT, 52);

    if (white_status == 1) {
      white1->setFont(SmallFont);
      black1->setFont(SmallFont);

      white1->print("You're in check!", CENTER, 32);
      black1->print("White in check!", CENTER, 32);
    } else if (black_status == 1) {
      white1->setFont(SmallFont);
      black1->setFont(SmallFont);

      black1->print("You're in check!", CENTER, 32);
      white1->print("Black in check!", CENTER, 32);

    }


    // Indicate active player
    if (timer->is_white_turn()) {
        white1->invert(true);
        black1->invert(false);
    } else {
        black1->invert(true);
        white1->invert(false);
    }
    
    board_display();
}

void Display::promotion() {
  enum selected_piece {
    CONFIRM,
    CHESS_BISHOP = 1,
    CHESS_ROOK = 2,
    CHESS_KNIGHT = 3,
    CHESS_QUEEN = 4
  };

  static selected_piece piece = CHESS_QUEEN;
  refresh = true;
  
  switch(piece) {
    case CHESS_QUEEN:
      white1->clrScr();
      white1->setFont(SmallFont);

      white1->print("Promoting to...", CENTER, 8);
      white1->print("<-   OK   ->", CENTER, 48);

      white1->setFont(BigFont);
      white1->print("QUEEN", CENTER, 24);

      if (button_states & BTN_LEFT)
        piece = CHESS_BISHOP;
      else if (button_states & BTN_SELECT)
        piece = CONFIRM;
      else if (button_states & BTN_RIGHT)
        piece = CHESS_KNIGHT;

      break;
    case CHESS_KNIGHT:
      white1->clrScr();
      white1->setFont(SmallFont);

      white1->print("Promoting to...", CENTER, 8);
      white1->print("<-   OK   ->", CENTER, 48);

      white1->setFont(BigFont);
      white1->print("KNIGHT", CENTER, 24);

      if (button_states & BTN_LEFT)
        piece = CHESS_QUEEN;
      else if (button_states & BTN_SELECT)
        piece = CONFIRM;
      else if (button_states & BTN_RIGHT)
        piece = CHESS_ROOK;

      break;
    case CHESS_ROOK:
      white1->clrScr();
      white1->setFont(SmallFont);

      white1->print("Promoting to...", CENTER, 8);
      white1->print("<-   OK   ->", CENTER, 48);

      white1->setFont(BigFont);
      white1->print("ROOK", CENTER, 24);

      if (button_states & BTN_LEFT)
        piece = CHESS_KNIGHT;
      else if (button_states & BTN_SELECT)
        piece = CONFIRM;
      else if (button_states & BTN_RIGHT)
        piece = CHESS_BISHOP;

      break;
    case CHESS_BISHOP:
      white1->clrScr();
      white1->setFont(SmallFont);

      white1->print("Promoting to...", CENTER, 8);
      white1->print("<-   OK   ->", CENTER, 48);

      white1->setFont(BigFont);
      white1->print("BISHOP", CENTER, 24);

      if (button_states & BTN_LEFT)
        piece = CHESS_ROOK;
      else if (button_states & BTN_SELECT)
        piece = CONFIRM;
      else if (button_states & BTN_RIGHT)
        piece = CHESS_QUEEN;

      break;
    case CONFIRM:
      screen = TIMER_CONF;
    default:
      break;
  }

}

void Display::untimed_display() {
  
  static uint32_t last_screen_update = 0;
  if (millis() - last_screen_update < 1000) {
    return;
  }
  last_screen_update = millis();

  white1->clrScr();
  black1->clrScr();

  white1->setFont(BigFont);
  black1->setFont(BigFont);

  if (chess_game->turn == CHESS_WHITE) {
    white1->invert(true);
    black1->invert(false);
    white1->print("YOUR", CENTER, 8);
    white1->print("TURN", CENTER, 24);
    black1->print("WHITES", CENTER, 8);
    black1->print("TURN", CENTER, 24);
  }
  else {
    white1->invert(false);
    black1->invert(true);
    black1->print("YOUR", CENTER, 8);
    black1->print("TURN", CENTER, 24);
    white1->print("BLACKS", CENTER, 8);
    white1->print("TURN", CENTER, 24);
  }

  char white_score_buffer[10];
  char black_score_buffer[10];
  chess_score_t white_score = chess_score(chess_game, CHESS_WHITE);
  chess_score_t black_score = chess_score(chess_game, CHESS_BLACK);

  std::sprintf(white_score_buffer, "Score: %d", (int)white_score);
  std::sprintf(black_score_buffer, "Score: %d", (int)black_score);

  chess_status_t white_status;
  chess_status_t black_status;
  chess_status(chess_game, &white_status, &black_status);

  Serial.print(white_status);
  Serial.print(" ");
  Serial.println(black_status);
  
  if (white_status == 1) {
    white1->setFont(SmallFont);
    black1->setFont(SmallFont);

    white1->print("You're in check!", CENTER, 40);
    black1->print("White in check!", CENTER, 40);
  } else if (black_status == 1) {
    white1->setFont(SmallFont);
    black1->setFont(SmallFont);

    black1->print("You're in check!", CENTER, 40);
    white1->print("Black in check!", CENTER, 40);

  }

  board_display();

  refresh = true;
}

void Display::game_over() {
  enum end_type {
    DETECT,
    TIMER,
    CHECKMATE,
    STALEMATE,
    PLAY_AGAIN
  };

  static end_type end_state = DETECT;
  refresh = true;

  chess_status_t white;
  chess_status_t black;
  uint8_t time_up;
  bool can_continue;

  switch(end_state) {
    case DETECT:
      // Check if time ran out
      time_up = timer->is_time_up();

      can_continue = chess_status(chess_game, &white, &black);

      Serial.print(white);
      Serial.print(" x ");
      Serial.println(black);

      if(time_up && timed_game) {
        end_state = TIMER;
      } else if (!can_continue) {
        // If someone is in checkmate
        if (white == 2 || black == 2) {
          end_state = CHECKMATE;
        } else if (white == 3 || black == 3) {
          end_state = STALEMATE;
        }
      }

      break;
    case TIMER:
      time_up = timer->is_time_up();
      timer->end();

      white1->clrScr();
      black1->clrScr();
      black1->setFont(SmallFont);
      white1->setFont(SmallFont);

      white1->print("Time ran out.", CENTER, 4);
      black1->print("Time ran out.", CENTER, 4);

      black1->setFont(BigFont);
      white1->setFont(BigFont);

      // White runs out of time, black wins
      if (time_up == 1) {
        black1->print("YOU WIN!", CENTER, 28);
        white1->print("LOSER!", CENTER, 28);

        black1->setFont(SmallFont);
        white1->setFont(SmallFont);

        black1->print("Press select", CENTER, 52);
        white1->print("Press select", CENTER, 52);
      } else {
        white1->print("YOU WIN!", CENTER, 28);
        black1->print("LOSER!", CENTER, 28);

        white1->setFont(SmallFont);
        black1->setFont(SmallFont);

        white1->print("Press select", CENTER, 52);
        black1->print("Press select", CENTER, 52);
      }
      
      end_state = PLAY_AGAIN;
      break;
    case CHECKMATE:

        white1->clrScr();
        black1->clrScr();
        white1->setFont(SmallFont);
        black1->setFont(SmallFont);

        white1->print("Checkmate!", CENTER, 4);
        black1->print("Checkmate!", CENTER, 4);

        white1->print("Press select", CENTER, 52);
        black1->print("Press select", CENTER, 52);

        white1->setFont(BigFont);
        black1->setFont(BigFont);

      // if (white == CHESS_CHECKMATE) {
        
      //   black1->print("YOU WIN!", CENTER, 28);
      //   white1->print("LOSER!", CENTER, 28);

      // } else if (black == CHESS_CHECKMATE) {
      //   white1->print("YOU WIN!", CENTER, 28);
      //   black1->print("LOSER!", CENTER, 28);

      // }

      end_state = PLAY_AGAIN;
      break;
    case STALEMATE:
        white1->clrScr();
        black1->clrScr();
        white1->setFont(SmallFont);
        black1->setFont(SmallFont);

        white1->print("Stalemate!", CENTER, 4);
        black1->print("Stalemate!", CENTER, 4);

        white1->print("Press select", CENTER, 52);
        black1->print("Press select", CENTER, 52);

        white1->setFont(BigFont);
        black1->setFont(BigFont);
        black1->print("TIE.", CENTER, 28);
        white1->print("TIE.", CENTER, 28);

        end_state = PLAY_AGAIN;
        break;
      case PLAY_AGAIN:
        if (button_states & (1 << 1)) {
          end_state = DETECT;
          screen = TIMER_CONF;
        }

        break;
      default:
        break;
  }
  if (end_state != DETECT) {
    screen = GAME_OVER;
  }
}

void Display::splash_screen()
{
  static bool splash_drawn = false;
  static uint32_t splash_time = 0;
  
  if (!splash_drawn) {
    white1->clrScr();
    white2->clrScr();
    black1->clrScr();
    black2->clrScr();

    white1->setFont(BigFont);
    black1->setFont(BigFont);

    // write wizard chess on main screens
    white1->print("Wizard", CENTER, 14);
    white1->print("Chess", CENTER, 34);

    black1->print("Wizard", CENTER, 14);
    black1->print("Chess", CENTER, 34);

    // Put oshe logo on side screens
    white2->drawBitmap(32, 0, oshe_logo, 64, 64);
    black2->drawBitmap(32, 0, oshe_logo, 64, 64);
    // black2->invert(true);

    splash_drawn = true;
    splash_time = millis();
    refresh = true;
    return;
  }

  // splash everywhere for 3 seconds
  if (millis() - splash_time > SPLASH_BREAK || (button_states & BTN_SELECT)) {
    screen = TIMER_CONF;
    button_states = 0;
  }
}

void Display::draw_invalid() 
{
  mOLED *cScr = NULL;
  chess_status_t white_status;
  chess_status_t black_status;
  chess_status(chess_game, &white_status, &black_status);
  bool checked = false;
  if (chess_game->turn == CHESS_WHITE) {
    cScr = white1;
    if (white_status == 1) {
      checked = true;
    }
  }
  else {
    cScr = black1;
    if (black_status == 1) {
      checked = true;
    }
  }

  if (!checked) {
    cScr->setFont(BigFont);
    cScr->invertText(false);
    cScr->print("BAD MOVE", CENTER, 16);
    cScr->setFont(SmallFont);
    cScr->print("RESET BOARD AS SHOWN", CENTER, 32);
  }
  else {
    cScr->setFont(BigFont);
    cScr->invertText(false);
    cScr->print("IN CHECK", CENTER, 16);
    cScr->setFont(SmallFont);
    cScr->print("Reset & dont lose!", CENTER, 32);
  }
}


void Display::board_reset() 
{
  mOLED *scr = white1;
  for (int i = 0; i < 2; i++) {
    if (i == 1) {
      scr = black1;
    }
    scr->clrScr();
    scr->setFont(BigFont);
    scr->invertText(false);
    scr->print("SET UP", CENTER, 4);
    scr->setFont(SmallFont);
    scr->print("the", CENTER, 20);
    scr->setFont(BigFont);
    scr->print("BOARD", CENTER, 28);
    scr->setFont(SmallFont);
    scr->print("set up the", CENTER, 44);
    scr->print("board as shown", CENTER, 52);
  }
  board_display();
}

void Display::debug_screen()
{
  if (refresh == true) {
    white1->setFont(SmallFont);
    white2->setFont(SmallFont);
    black1->setFont(SmallFont);
    black2->setFont(SmallFont);  
    int x = 0;
    int y = 0;
    int bit = 0;
    white1->clrScr();
    white2->clrScr();
    uint64_t data = debug_data[0];
    for (int i = 0; i < 64; i++) {
      if (i == 32) {
        data = debug_data[1];
      }
      y = ((i) / 8);
      x = 7 - ((i) % 8);
      if (((data) & 1)) {
        white1->drawBitmap(x * 16, y * 8, debug_tile, 16, 8);
      }
      else {
        white1->printNumI((63 - i), x*16, y*8);
      }
      data = data >> 1;
    }
    // Serial.println(debug_data, BIN);
    // Serial.println(0xFFFFFFFFFFFFFFFE, BIN);
    if (chess_game != NULL) {
      for (int i = 0; i < 64; i++) {
        chess_id_t current = chess_game->board[i];
        int y = ((63 - i) / 8);
        int x = 7 - ((63 - i) % 8);
        switch (CHESS_TEAM(current)) {
          case CHESS_WHITE:
            switch (CHESS_TYPE(current)) {
              case CHESS_PAWN:
                white2->print("wp", x*16, y*8);
                break;
              case CHESS_BISHOP:
                white2->print("wB", x*16, y*8);
                break;
              case CHESS_ROOK:
                white2->print("wR", x*16, y*8);
                break;
              case CHESS_KNIGHT:
                white2->print("wH", x*16, y*8);
                break;
              case CHESS_QUEEN:
                white2->print("WQ", x*16, y*8);
                break;
              case CHESS_KING:
                white2->print("WK", x*16, y*8);
                break;
              default:
                white2->drawBitmap(x * 16, y * 8, debug_tile, 16, 8);
            }
          break;
          case CHESS_BLACK:
            switch (CHESS_TYPE(current)) {
              case CHESS_PAWN:
                white2->print("bp", x*16, y*8);
                break;
              case CHESS_BISHOP:
                white2->print("bB", x*16, y*8);
                break;
              case CHESS_ROOK:
                white2->print("bR", x*16, y*8);
                break;
              case CHESS_KNIGHT:
                white2->print("bH", x*16, y*8);
                break;
              case CHESS_QUEEN:
                white2->print("BQ", x*16, y*8);
                break;
              case CHESS_KING:
                white2->print("BK", x*16, y*8);
                break;
              default:
                white2->drawBitmap(x * 16, y * 8, debug_tile, 16, 8);
            }
          break;
          default:
            white2->drawBitmap(x * 16, y * 8, debug_tile, 16, 8);
        }
      }
    }
  }
}

//////// Public Functions

Display::Display()
{
  Serial.println("Init White 1");
  white1 = new mOLED(WHITE_SCR_1_SDA, WHITE_SCR_1_SCL);
  Serial.println("Init White 2");
  white2 = new mOLED(WHITE_SCR_2_SDA, WHITE_SCR_2_SCL);
  Serial.println("Init Black 1");
  black1 = new mOLED(BLACK_SCR_1_SDA, BLACK_SCR_1_SCL);
  Serial.println("Init Black 2");
  black2 = new mOLED(BLACK_SCR_2_SDA, BLACK_SCR_2_SCL);

  if (!white1->begin(SSD1306_128X64))
  {
    Serial.println("Failed to start White 1");
    white1 = NULL;
  }
  Wire.end();
  // white2 = NULL;
  // black1 = NULL;
  // black2 = NULL;
  if (!white2->begin(SSD1306_128X64))
  {
    Serial.println("Failed to start White 2");
    white2 = NULL;
  }
  Wire.end();
  if (!black1->begin(SSD1306_128X64))
  {
    Serial.println("Failed to start Black 1");
    black1 = NULL;
  }
  Wire.end();
  if (!black2->begin(SSD1306_128X64))
  {
    Serial.println("Failed to start Black 2");
    black2 = NULL;
  }
  Wire.end();
  
  timer = NULL;
  chess_game = NULL;

  invalid_move = false;

  // screen = SPLASH; should be here normally?
  screen = SPLASH;
  debug_data[0] = 0;
  debug_data[1] = 0;
  refresh = true;
  Serial.println("Displays Started");
}

void Display::update()
{
  // Serial.println("Calling Screen Function");
  switch(screen) {
    case TIMER_CONF:
      timer_configuration();
      break;
    case BOARD_DISPLAY:
      board_display();
      break;
    case TIMER_DISPLAY:
      timer_display();
      break;
    case UNTIMED_DISPLAY:
      untimed_display();
      break;
    case PROMOTION:
      promotion();
      break;
    case GAME_OVER:
      game_over();
      break;
    case SPLASH:
      splash_screen();
      break;
    case DEBUG:
      debug_screen();
      break;
    case BOARD_RESET:
      board_reset();
      break;
    default:
      Serial.println("INVALID SCREEN STATE");
  }

  // Invalid Moves
  if (invalid_move) {
    draw_invalid();
  }
  int i = BTN_LEFT;

  // Refresh Screens
  // Serial.println("Calling Update Functions");
  if (white1 != NULL && refresh) {
    // Serial.println("Updating white 1");
    white1->update();
  }
  // delay(200);
  if (white2 != NULL && refresh) {
    // Serial.println("Updating white 2");
    white2->update();
  }
  // delay(200);
  if (black1 != NULL && refresh) {
    // Serial.println("Updating black 1");
    black1->update();
  }
  // delay(200);
  if (black2 != NULL && refresh) {
    // Serial.println("Updating black 2");
    black2->update();
  }
  // delay(200);
  refresh = false;
}

void Display::set_screen(Screen scr)
{
  // TODO: validate
  screen = scr;
  refresh = true;
}

Screen Display::get_screen() {
  return screen;
}

void Display::set_timer(Timer *t) {
  timer = t;
  refresh = true;
}

void Display::set_chess_board(chess_game_t *game) {
  chess_game = game;
}

void Display::set_invalid_mode(bool set)
{
  invalid_move = set;
}

void Display::set_debug_data(uint32_t dataLow, uint32_t dataHigh)
{
  debug_data[0] = dataLow;
  debug_data[1] = dataHigh;
  if (screen == DEBUG) {
    refresh = true;
  }
}

void Display::force_refresh() {
  refresh = true;
}

///////// Custom OLED functions

mOLED::mOLED(uint8_t data_pin, uint8_t sclk_pin, uint8_t rst_pin)
  : OLED(data_pin, sclk_pin, rst_pin) 
{
  // calls parent constructor
}
mOLED::mOLED(uint8_t data_pin, uint8_t sclk_pin)
	: OLED(data_pin, sclk_pin) 
{
  // calls parent constructor
}

void mOLED::update() 
{
  // Switch the selected Wire pins to current class
  Wire.end();
  _initTWI();
  // Serial.print("Reset Wire Interface with args: ");
  // Serial.print(_sda_pin);
  // Serial.print(" ");
  // Serial.print(_scl_pin);
  // Serial.print(" ");
  // Serial.println(TWI_FREQ);
  OLED::update();
  Wire.end();
}

void mOLED::invert(bool mode) {
  Wire.end();
  _initTWI();
  OLED::invert(mode);
  Wire.end();
}

void mOLED::drawInvertedBitmap(int x, int y, uint8_t* bitmap, int sx, int sy, bool inverted) {
  OLED::drawBitmap(x, y, bitmap, sx, sy);
  if (inverted) {
    int bit;
    byte data;
    for (int cy=0; cy<sy; cy++)
    {
      bit= cy % 8;
      for(int cx=0; cx<sx; cx++)
      {
        invPixel(x+cx, y+cy);
      }
    }      
  }
}

uint8_t* mOLED::_getScreen()
{
  // return the ptr to the screen buffer
  return scrbuf;
}
