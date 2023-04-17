#include "ClockDisplay.h"
#include "TinyFont.h"

const byte row0 = 2+0*10;
const byte row1 = 2+1*10;
const byte row2 = 2+2*10;

ClockDisplay::ClockDisplay(PxMATRIX* display){
  this->display = display;
  clearDisplay();
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