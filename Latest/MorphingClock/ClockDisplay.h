#ifndef CLOCKDISPLAY_H
#define CLOCKDISPLAY_H

#include <PxMatrix.h> 

class ClockDisplay{
  public:
    ClockDisplay();
    ClockDisplay(PxMATRIX* display);
    void displayNetworkInfo(char accessPointName[], char accessPointPassword[], char accessPointIP[]);
    void displayConfigInfo(char timezone[], char timeFormat[]);
    void clearDisplay();
    void showText(char text[]);

  private:
    PxMATRIX* display;
    /*struct Digit{
      byte _value;
      uint16_t _color;
      uint16_t xOffset;
      uint16_t yOffset;      
    }*/

    /*Digit digits[Digit {0, 63 - 1 - 9*1, 8, display.color565(0, 0, 255)}, 
                 Digit {0, 63 - 1 - 9*2, 8, display.color565(0, 0, 255)}, 
                 Digit {0, 63 - 4 - 9*3, 8, display.color565(0, 0, 255)}, 
                 Digit {0, 63 - 4 - 9*4, 8, display.color565(0, 0, 255)}, 
                 Digit {0, 63 - 7 - 9*5, 8, display.color565(0, 0, 255)}, 
                 Digit {0, 63 - 7 - 9*6, 8, display.color565(0, 0, 255)}]*/

    //void drawPixel(uint16_t x, uint16_t y, uint16_t c);
    /*void drawFillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c);
    void drawLine(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, uint16_t c);
    void drawSeg(byte seg);
    void Morph2();
    void Morph3();
    void Morph4();
    void Morph5();
    void Morph6();
    void Morph7();
    void Morph8();
    void Morph9();
    void Morph0();
    void Morph1();*/
};
#endif