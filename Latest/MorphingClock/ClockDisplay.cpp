#include <Ticker.h>

#include "ClockDisplay.h"
#include "TinyFont.h"

const byte row0 = 2+0*10;
const byte row1 = 2+1*10;
const byte row2 = 2+2*10;

Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

// Pins for LED MATRIX
//PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

ClockDisplay::ClockDisplay(){
}

ClockDisplay::ClockDisplay(PxMATRIX* display){
  
  this->display = display;

  this->display->begin(16);

  display_ticker.attach_ms(2, [](PxMATRIX *displayToUse) {
    displayToUse->display(70);
  }, display);
  
  //clearDisplay();
  this->display->setTextColor(this->display->color565(0, 0, 255));
}

void ClockDisplay::displayNetworkInfo(char accessPointName[], char accessPointPassword[], char accessPointIP[]){
  clearDisplay();
  display->setCursor(1, row0); 
  display->print("AP");
  display->setCursor(1+10, row0); 
  display->print(":");
  display->setCursor(1+10+5, row0); 
  display->print(accessPointName);

  display->setCursor(1, row1); 
  display->print("Pw");
  display->setCursor(1+10, row1); 
  display->print(":");
  display->setCursor(1+10+5, row1); 
  display->print(accessPointPassword);

  display->setCursor(1, row2); 
  display->print("IP");
  display->setCursor(1+10, row2); 
  display->print(":");
  TFDrawText (display, accessPointIP, 1+10+5, row2 + 1, display->color565(0, 0, 255));
}

void ClockDisplay::displayConfigInfo(char timezone[], char timeFormat[]){
  clearDisplay();
  display->setCursor(1, row0); 
  display->print("Connected!");

  display->setCursor(1, row1); 
  display->print("Zone:");
  display->print(timezone);

  display->setCursor(1, row2); 
  display->print("Military:");
  display->print(timeFormat);
}

void ClockDisplay::clearDisplay(){
  display->fillScreen(this->display->color565(0, 0, 0));
}

void ClockDisplay::showText(char *text){
  clearDisplay();
  display->setCursor(2, row0);
  display->print(text);
}

/*void ClockDisplay::drawPixel(uint16_t x, uint16_t y, uint16_t c)
{
  display->drawPixel(xOffset + x, height - (y + yOffset), c);
}*/