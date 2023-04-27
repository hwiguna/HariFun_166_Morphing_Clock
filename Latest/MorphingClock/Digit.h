#ifndef DIGIT_H
#define DIGIT_H

#include <Arduino.h>

#include <PxMatrix.h> // https://github.com/2dom/PxMatrix

class Digit {
  
  public:
    Digit();
    Digit(PxMATRIX* d, byte value, uint16_t x_offset, uint16_t y_offset, bool small, uint16_t colour);
    void draw(byte value);
    void clear();
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

    void draw_pixel(uint16_t x, uint16_t y, uint16_t c);
    void draw_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c);
    void draw_line(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, uint16_t c);
    void draw_seg(byte seg);
    void draw_seg(byte seg, uint16_t seg_colour);
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
