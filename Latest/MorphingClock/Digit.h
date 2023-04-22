#ifndef DIGIT_H
#define DIGIT_H

#include <Arduino.h>

#include <PxMatrix.h> // https://github.com/2dom/PxMatrix

class Digit {
  
  public:
    Digit();
    Digit(PxMATRIX* d, byte value, uint16_t xo, uint16_t yo, bool small, uint16_t color);
    void draw(byte value);
    void morph(byte new_value);
    byte value() const;
    void draw_colon(uint16_t c);
    
  private:
    PxMATRIX* display;
    byte current_value;
    uint16_t colour;
    uint16_t x_offset;
    uint16_t y_offset;
    int anim_speed = 30;
    
    int seg_height;
    int seg_width;

    static const byte sA = 0;
    static const byte sB = 1;
    static const byte sC = 2;
    static const byte sD = 3;
    static const byte sE = 4;
    static const byte sF = 5;
    static const byte sG = 6;
    static const int seg_height_small = 3;
    static const int seg_height_large = 6;
    static const uint16_t height = 31;
    static const uint16_t width = 63;

    void draw_pixel(uint16_t x, uint16_t y, uint16_t c);
    void draw_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c);
    void draw_line(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, uint16_t c);
    void draw_seg(byte seg);
    void morph2();
    void morph3();
    void morph4();
    void morph5();
    void morph6();
    void morph7();
    void morph8();
    void morph9();
    void morph0();
    void morph1();
};

#endif
