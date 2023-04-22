#ifndef CLOCKDISPLAY_H
#define CLOCKDISPLAY_H

#include <PxMatrix.h> 

#include "Digit.h"

class ClockDisplay{
  public:
    ClockDisplay();
    void displayNetworkInfo(const char accessPointName[], const char accessPointPassword[], const char accessPointIP[]);
    void displayConfigInfo(const char timezone[], const char timeFormat[]);
    void clearDisplay();
    void showText(const char text[]);
    void showTime(int hh, int mm, int ss, bool isPM, bool military);
    void morphTime(int hh, int mm, int ss, bool isPM, bool military);
    void show_weather(float current_temp, float min_temp);

  private:
    uint16_t timeColour;
    Digit digit0;
    Digit digit1;
    Digit digit2;
    Digit digit3;
    Digit digit4;
    Digit digit5;  

    void showAMPM(bool isPM); 
};
#endif