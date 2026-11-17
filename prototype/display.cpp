#include <OLED_I2C.h>
#include "display.h"

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

void Display::timer_configuration()
{

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

}

void Display::debug_screen()
{
  if (refresh == true) {
    int x = 0;
    int y = 0;
    int bit = 0;
    white1->clrScr();
    uint64_t data = debug_data;
    for (int i = 0; i < 64; i++) {
      y = i / 8;
      x = i % 8;
      if (((data) & 1)) {
        // white1->drawBitmap(x * 16, y * 8, debug_tile, 16, 8);
        white1->printNumI(10, 24, 0);
        white1->print(":", 56, 0);
        white1->printNumI(50, 72, 0);
        white1->invert(true);
        // white1->print(":")
        // white1->drawRect(x*16, y*8, x*16 + 16, y*8 + 8);
      }
      data = data >> 1;
    }
    Serial.println(debug_data, BIN);
    Serial.println(0xFFFFFFFFFFFFFFFE, BIN);
  }
}

//////// Public Functions

Display::Display()
{
  Serial.println("Init White 1");
  white1 = new OLED(WHITE_SCR_1_SDA, WHITE_SCR_1_SCL);
  // Serial.println("Init White 2");
  // white2 = new OLED(WHITE_SCR_2_SDA, WHITE_SCR_2_SCL);
  // Serial.println("Init Black 1");
  // black1 = new OLED(BLACK_SCR_1_SDA, BLACK_SCR_1_SCL);
  // Serial.println("Init Black 2");
  // black2 = new OLED(BLACK_SCR_2_SDA, BLACK_SCR_2_SCL);

  white2 = NULL;
  black1 = NULL;
  black2 = NULL;
  if (!white1->begin(SSD1306_128X64))
  {
    Serial.println("Failed to start White 1");
    white1 = NULL;
  }
  white1->setFont(BigFont);
  // if (!white2->begin(SSD1306_128X64))
  // {
  //   Serial.println("Failed to start White 2");
  //   white2 = NULL;
  // }
  // if (!black1->begin(SSD1306_128X64))
  // {
  //   Serial.println("Failed to start Black 1");
  //   black1 = NULL;
  // }
  // if (!black2->begin(SSD1306_128X64))
  // {
  //   Serial.println("Failed to start Black 2");
  //   black2 = NULL;
  // }
  
  timer = NULL;
  screen = DEBUG;
  debug_data = 1;
  refresh = true;
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
  // Serial.println("Calling Update Functions");
  if (white1 != NULL && refresh) 
    white1->update();
  if (white2 != NULL && refresh) 
    white2->update();
  if (black1 != NULL && refresh) 
    black1->update();
  if (black2 != NULL && refresh) 
    black2->update();
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

void Display::set_invalid_mode(bool set)
{
  invaid_move = set;
}

void Display::set_debug_data(uint64_t data)
{
  debug_data = data;
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
  _initTWI();
  OLED::update();
}

uint8_t* mOLED::_getScreen()
{
  // return the ptr to the screen buffer
  return scrbuf;
}
