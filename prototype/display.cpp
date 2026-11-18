#include <OLED_I2C.h>
#include <Wire.h>
#include "display.h"

extern uint8_t SmallFont[];

#define WHITE_SCR_1_SDA 18
#define WHITE_SCR_1_SCL 19
#define WHITE_SCR_2_SDA 22
#define WHITE_SCR_2_SCL 23

#define BLACK_SCR_1_SDA 27
#define BLACK_SCR_1_SCL 26
#define BLACK_SCR_2_SDA 25
#define BLACK_SCR_2_SCL 33

extern uint8_t button_states;
extern uint8_t BigFont[];

//////// Bitmaps

static uint8_t debug_tile[] = {
  0b00000000, 0b01111110,
  0b01111110, 0b01111110,
  0b01111110, 0b01111110,
  0b01111110, 0b01111110,
  0b01111110, 0b01111110,
  0b01111110, 0b01111110,
  0b01111110, 0b01111110,
  0b01111110, 0b00000000
};

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
    SET
  };

  uint32_t set_time = 0;
  uint32_t set_increment = 0;

  static configPhase phase = FIRST;
  // Set rapid as default config 
  

  white1->setFont(BigFont);

  switch(phase) {
    
    case FIRST:
      white1->clrScr();
      white1->print("Set Time", CENTER, 12);
      white1->setFont(SmallFont);
      white1->print("Press Select", CENTER, 48);
      
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

      if (button_states & (1 << 0)) 
        phase = THREE_TWO;
      else if (button_states & (1 << 1)) {
        phase = SET;
        set_time = 10;
        set_increment = 0;
      } else if (button_states & (1 << 2))
        phase = BLITZ;

      break;
    case FIVE:
      white1->clrScr();
      white1->print("BLITZ 5", CENTER, 4);
      white1->setFont(SmallFont);
      white1->print("5 minutes", CENTER, 24);
      white1->print("<-   OK   ->", CENTER, 52);

      if (button_states & (1 << 0)) 
        phase = RAPID;
      else if (button_states & (1 << 1)) {
        phase = SET;
        set_time = 5;
        set_increment = 0;
      } else if (button_states & (1 << 2))
        phase = BLITZ;

      break;
    
    case BLITZ:
      white1->clrScr();
      white1->print("BLITZ 3", CENTER, 4);
      white1->setFont(SmallFont);
      white1->print("3 minutes", CENTER, 24);
      white1->print("<-   OK   ->", CENTER, 52);

      if (button_states & (1 << 0)) 
        phase = FIVE;
      else if (button_states & (1 << 1)) {
        phase = SET;
        set_time = 3;
        set_increment = 0;
      } else if (button_states & (1 << 2))
        phase = BULLET;

      
      break;
    case BULLET:
      white1->clrScr();
      white1->print("BULLET", CENTER, 4);
      white1->setFont(SmallFont);
      white1->print("1 minute", CENTER, 24);
      white1->print("<-   OK   ->", CENTER, 52);

      if (button_states & (1 << 0)) 
        phase = BLITZ;
      else if (button_states & (1 << 1)) {
        phase = SET;
        set_time = 1;
        set_increment = 0;
      } else if (button_states & (1 << 2))
        phase = FIFTEEN_TEN;

      break;
    case FIFTEEN_TEN:
      white1->clrScr();
      white1->print("15|10", CENTER, 4);
      white1->setFont(SmallFont);
      white1->print("15 min + 10 sec", CENTER, 24);
      white1->print("<-   OK   ->", CENTER, 52);

      if (button_states & (1 << 0)) 
        phase = BULLET;
      else if (button_states & (1 << 1)) {
        phase = SET;
        set_time = 15;
        set_increment = 10;
      } else if (button_states & (1 << 2))
        phase = THREE_TWO;

      break;
    case THREE_TWO:
      white1->clrScr();
      white1->print("3|2", CENTER, 4);
      white1->setFont(SmallFont);
      white1->print("3 min + 2 sec", CENTER, 24);
      white1->print("<-   OK   ->", CENTER, 52);

      if (button_states & (1 << 0)) 
        phase = FIFTEEN_TEN;
      else if (button_states & (1 << 1)){ 
        phase = SET;
        set_time = 3;
        set_increment = 2;
      } else if (button_states & (1 << 2))
        phase = RAPID;

      break;
    case SET:
      // Pass timer settings to timer
      timer->reset(set_time, set_increment);
      timer->set_configured();

      //screen = TIMER_DISPLAY;
      break;
    default:
      break;
  }

  button_states = 0;
  refresh = true;

}

void Display::board_display()
{

}

void Display::timer_display()
{
    if (timer == NULL)
        return;

    // if (white1 == NULL || black1 == NULL)
    //     return;
    
    static uint32_t last_screen_update = 0;
    if (millis() - last_screen_update < 250) return;
    
    last_screen_update = millis();
    refresh = true;

    uint32_t white_time = timer->get_white_time();
    uint32_t black_time = timer->get_black_time();

    uint32_t white_total_seconds = (white_time + 999) / 1000;
    uint32_t black_total_seconds = (black_time + 999) / 1000;

    uint32_t white_minutes = white_total_seconds / 60;
    uint32_t white_seconds = white_total_seconds % 60;

    uint32_t black_minutes = black_total_seconds / 60;
    uint32_t black_seconds = black_total_seconds % 60;

    white1->setFont(BigFont); 

    white1->clrScr();

    // White clock (top)
    white1->print("W:", 0, 0);
    white1->printNumI(white_minutes, 32, 0);
    white1->print(":", 56, 0);

    if (white_seconds < 10) {
      white1->printNumI(0, 72, 0);
      white1->printNumI(white_seconds, 88, 0);
    } else {
      white1->printNumI(white_seconds, 72, 0);
    }

    // Black clock (bottom)
    white1->print("B:", 0, 32);
    white1->printNumI(black_minutes, 32, 32);
    white1->print(":", 56, 32);

    if (black_seconds < 10) {
      white1->printNumI(0, 72, 32);
      white1->printNumI(black_seconds, 88, 32);
    } else {
      white1->printNumI(black_seconds, 72, 32);
    }

    // Indicate active player
  
    if (timer->is_white_turn())
        white1->invert(true);
    else
        white1->invert(false);
    
    // // White clock
    // white1->clrScr();
    // white1->printNumI(white_minutes, 24, 0);
    // white1->print(":", 56, 0);

    // if (white_seconds < 10) {
    //   white1->printNumI(0, 72, 0);
    //   white1->printNumI(white_seconds, 88, 0);
    // } else {
    //   white1->printNumI(white_seconds, 72, 0);
    // }
    // // Black clock
    // black1->clrScr();
    // black1->printNumI(black_minutes, 24, 0);
    // black1->print(":", 56, 0);

    // if (black_seconds < 10) {
    //   black1->printNumI(0, 72, 0);
    //   black1->printNumI(black_seconds, 88, 0);
    // } else {
    //   black1->printNumI(black_seconds, 72, 0)
    // }

}

void Display::splash_screen()
{
  white1->setFont(SmallFont);
  white2->setFont(SmallFont);
  black1->setFont(SmallFont);
  black2->setFont(SmallFont);  
  static int num = 0;
  num++;
  Serial.println("White 1");
  white1->clrScr();
  white1->print("White 1", 0, 0);
  white1->printNumI(num, 20, 20);
  Serial.println("White 2");
  white2->clrScr();
  white2->print("White 2", 0, 0);
  white2->printNumI(num, 20, 20);
  Serial.println("Black 1");
  black1->clrScr();
  black1->print("Black 1", 0, 0);
  Serial.println("Black 2");
  black1->printNumI(num, 20, 20);
  black2->clrScr();
  black2->print("Black 2", 0, 0);
  black2->printNumI(num, 20, 20);
  refresh=true;
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
    uint64_t data = debug_data[0];
    for (int i = 0; i < 64; i++) {
      if (i == 32) {
        data = debug_data[1];
      }
      y = ((63 - i) / 8);
      x = 7 - ((63 - i) % 8);
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
        int y = ((i) / 8);
        int x = 7 - ((i) % 8);
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

  screen = TIMER_CONF;
  debug_data[0] = 0;
  debug_data[1] = 0;
  refresh = true;
  Serial.println("Displays Started");
}

void Display::update()
{
  Serial.println("Calling Screen Function");
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
    case SPLASH:
      splash_screen();
      break;
    case DEBUG:
      debug_screen();
      break;
    default:
      Serial.println("INVALID SCREEN STATE");
  }

  // Invalid Moves

  // Refresh Screens
  Serial.println("Calling Update Functions");
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

void Display::set_timer(Timer *t) {
  timer = t;
  refresh = true;
}

void Display::set_chess_board(chess_game_t *game) {
  chess_game = game;
}

void Display::set_invalid_mode(bool set)
{
  invaid_move = set;
}

void Display::set_debug_data(uint32_t dataLow, uint32_t dataHigh)
{
  debug_data[0] = dataLow;
  debug_data[1] = dataHigh;
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

uint8_t* mOLED::_getScreen()
{
  // return the ptr to the screen buffer
  return scrbuf;
}
