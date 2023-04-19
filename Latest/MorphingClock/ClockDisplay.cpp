#include <Ticker.h>

#include "ClockDisplay.h"
#include "TinyFont.h"
#include "Digit.h"

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

//=== SEGMENTS ===
//Digit segments[6];
/*Digit digit0;
Digit digit1;
Digit digit2;
Digit digit3;
Digit digit4;
Digit digit5;*/

ClockDisplay::ClockDisplay(){
  
}

ClockDisplay::ClockDisplay(PxMATRIX* display){
  
  this->display = display;

  this->display->begin(16);

  digit0 = Digit(display, 0, 63 - 1 - 9*1, 8, display->color565(0, 0, 255));
  digit1 = Digit(display, 0, 63 - 1 - 9*2, 8, display->color565(0, 0, 255));
  digit2 = Digit(display, 0, 63 - 4 - 9*3, 8, display->color565(0, 0, 255));
  digit3 = Digit(display, 0, 63 - 4 - 9*4, 8, display->color565(0, 0, 255));
  digit4 = Digit(display, 0, 63 - 7 - 9*5, 8, display->color565(0, 0, 255));
  digit5 = Digit(display, 0, 63 - 7 - 9*6, 8, display->color565(0, 0, 255));

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

void ClockDisplay::showTime(int hh, int mm, int ss){
  clearDisplay();
  digit0.Draw(ss % 10);
  digit1.Draw(ss / 10);
  digit1.DrawColon(display->color565(0, 0, 255));
  digit2.Draw(mm % 10);
  digit3.Draw(mm / 10);
  digit3.DrawColon(display->color565(0, 0, 255));
  digit4.Draw(hh % 10);
  digit5.Draw(hh / 10);
}

void ClockDisplay::morphTime(int hh, int mm, int ss){
  
  int s0 = ss % 10;
  int s1 = ss / 10;
  if (s0!=digit0.Value()) digit0.Morph(s0);
  if (s1!=digit1.Value()) digit1.Morph(s1);
        
  int m0 = mm % 10;
  int m1 = mm / 10;
  if (m0!=digit2.Value()) digit2.Morph(m0);
  if (m1!=digit3.Value()) digit3.Morph(m1);
      
  int h0 = hh % 10;
  int h1 = hh / 10;
  if (h0!=digit4.Value()) digit4.Morph(h0);
  if (h1!=digit5.Value()) digit5.Morph(h1);
}
/*void ClockDisplay::drawPixel(uint16_t x, uint16_t y, uint16_t c)
{
  display->drawPixel(xOffset + x, height - (y + yOffset), c);
}*/