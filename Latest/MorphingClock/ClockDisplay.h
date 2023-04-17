#ifndef CLOCKDISPLAY_H
#define CLOCKDISPLAY_H

#include <PxMatrix.h> 

class ClockDisplay{
  public:
    ClockDisplay(PxMATRIX* display);
    void displayNetworkInfo(char accessPointName[], char accessPointPassword[], char accessPointIP[]);
    void displayConfigInfo(char timezone[], char timeFormat[]);
    void clearDisplay();
    void showText(char text[]);

  private:
    PxMATRIX* display;
    struct Digit{
      
    }

};
#endif