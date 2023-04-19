#ifndef CLOCKDISPLAY_H
#define CLOCKDISPLAY_H

#include <PxMatrix.h> 

#include "Digit.h"

class ClockDisplay{
  public:
    ClockDisplay();
    void displayNetworkInfo(char accessPointName[], char accessPointPassword[], char accessPointIP[]);
    void displayConfigInfo(char timezone[], char timeFormat[]);
    void clearDisplay();
    void showText(char text[]);
    void showTime(int hh, int mm, int ss);
    void morphTime(int hh, int mm, int ss);

  private:
    Digit digit0;
    Digit digit1;
    Digit digit2;
    Digit digit3;
    Digit digit4;
    Digit digit5;   
};
#endif