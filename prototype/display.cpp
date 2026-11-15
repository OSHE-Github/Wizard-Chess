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
        white1->drawBitmap(x * 16, y * 8, debug_tile, 16, 8);
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
  

  screen = DEBUG;
  debug_data = 0;
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
  // Serial.println("Calling Update Functions");
  if (white1 != NULL && refresh) 
    white1->update();
  // delay(1000);
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
