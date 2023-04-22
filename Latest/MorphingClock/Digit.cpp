#include "Digit.h"

/*static const byte sA = 0;
static const byte sB = 1;
static const byte sC = 2;
static const byte sD = 3;
static const byte sE = 4;
static const byte sF = 5;
static const byte sG = 6;
static const int seg_height_small = 3;
static const int seg_height_large = 6;
static const uint16_t height = 31;
static const uint16_t width = 63;*/

static const byte digitBits[] = {
  B11111100, // 0 ABCDEF--
  B01100000, // 1 -BC-----
  B11011010, // 2 AB-DE-G-
  B11110010, // 3 ABCD--G-
  B01100110, // 4 -BC--FG-
  B10110110, // 5 A-CD-FG-
  B10111110, // 6 A-CDEFG-
  B11100000, // 7 ABC-----
  B11111110, // 8 ABCDEFG-
  B11110110, // 9 ABCD_FG-
};

uint16_t black;

Digit::Digit(){

}

Digit::Digit(PxMATRIX* d, byte value, uint16_t xo, uint16_t yo, bool small, uint16_t color) {
  display = d;
  current_value = value;
  x_offset = xo;
  y_offset = yo;
  colour = color;

  if (small){
    seg_height = seg_height_small;
    seg_width = seg_height_small;
  }
  else{
    seg_height = seg_height_large;
    seg_width = seg_height_large;
  }
}

byte Digit::value() const{
  return current_value;
}

void Digit::draw_pixel(uint16_t x, uint16_t y, uint16_t c)
{
  display->drawPixel(x_offset + x, height - (y + y_offset), c);
}

void Digit::draw_line(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, uint16_t c)
{
  display->drawLine(x_offset + x, height - (y + y_offset), x_offset + x2, height - (y2 + y_offset), c);
}

void Digit::draw_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c)
{
  display->fillRect(x_offset + x, height - (y + y_offset), w,h, c);
}

void Digit::draw_colon(uint16_t c)
{
  // Colon is drawn to the left of this digit
  draw_fill_rect(-3, seg_height-1, 2,2, c);
  draw_fill_rect(-3, seg_height+1+3, 2,2, c);
}

void Digit::draw_seg(byte seg)
{
  switch (seg) {
    case sA: 
      draw_line(1, seg_height * 2 + 2, seg_width, seg_height * 2 + 2, colour); 
      break;
    case sB: 
      draw_line(seg_width + 1, seg_height * 2 + 1, seg_width + 1, seg_height + 2, colour); 
      break;
    case sC: 
      draw_line(seg_width + 1, 1, seg_width + 1, seg_height, colour); 
      break;
    case sD: 
      draw_line(1, 0, seg_width, 0, colour); 
      break;
    case sE: 
      draw_line(0, 1, 0, seg_height, colour); 
      break;
    case sF: 
      draw_line(0, seg_height * 2 + 1, 0, seg_height + 2, colour); 
      break;
    case sG: 
      draw_line(1, seg_height + 1, seg_width, seg_height + 1, colour); 
      break;
  }
}

void Digit::draw(byte value) {
  byte pattern = digitBits[value];
  if (bitRead(pattern, 7)) draw_seg(sA);
  if (bitRead(pattern, 6)) draw_seg(sB);
  if (bitRead(pattern, 5)) draw_seg(sC);
  if (bitRead(pattern, 4)) draw_seg(sD);
  if (bitRead(pattern, 3)) draw_seg(sE);
  if (bitRead(pattern, 2)) draw_seg(sF);
  if (bitRead(pattern, 1)) draw_seg(sG);
  current_value = value;
}

void Digit::morph2() {
  // TWO
  for (int i = 0; i <= seg_width; i++)
  {
    if (i < seg_width) {
      draw_pixel(seg_width - i, seg_height * 2 + 2, colour);
      draw_pixel(seg_width - i, seg_height + 1, colour);
      draw_pixel(seg_width - i, 0, colour);
    }

    draw_line(seg_width + 1 - i, 1, seg_width + 1 - i, seg_height, black);
    draw_line(seg_width - i, 1, seg_width - i, seg_height, colour);
    delay(anim_speed);
  }
}

void Digit::morph3() {
  // THREE
  for (int i = 0; i <= seg_width; i++)
  {
    draw_line(0 + i, 1, 0 + i, seg_height, black);
    draw_line(1 + i, 1, 1 + i, seg_height, colour);
    delay(anim_speed);
  }
}

void Digit::morph4() {
  // FOUR
  for (int i = 0; i < seg_width; i++)
  {
    draw_pixel(seg_width - i, seg_height * 2 + 2, black); // Erase A
    draw_pixel(0, seg_height * 2 + 1 - i, colour); // Draw as F
    draw_pixel(1 + i, 0, black); // Erase D
    delay(anim_speed);
  }
}

void Digit::morph5() {
  // FIVE
  for (int i = 0; i < seg_width; i++)
  {
    draw_pixel(seg_width + 1, seg_height + 2 + i, black); // Erase B
    draw_pixel(seg_width - i, seg_height * 2 + 2, colour); // Draw as A
    draw_pixel(seg_width - i, 0, colour); // Draw D
    delay(anim_speed);
  }
}

void Digit::morph6() {
  // SIX
  for (int i = 0; i <= seg_width; i++)
  {
    // Move C right to left
    draw_line(seg_width - i, 1, seg_width - i, seg_height, colour);
    if (i > 0) draw_line(seg_width - i + 1, 1, seg_width - i + 1, seg_height, black);
    delay(anim_speed);
  }
}

void Digit::morph7() {
  // SEVEN
  for (int i = 0; i <= (seg_width + 1); i++)
  {
    // Move E left to right
    draw_line(0 + i - 1, 1, 0 + i - 1, seg_height, black);
    draw_line(0 + i, 1, 0 + i, seg_height, colour);

    // Move F left to right
    draw_line(0 + i - 1, seg_height * 2 + 1, 0 + i - 1, seg_height + 2, black);
    draw_line(0 + i, seg_height * 2 + 1, 0 + i, seg_height + 2, colour);

    // Erase D and G gradually
    draw_pixel(1 + i, 0, black); // D
    draw_pixel(1 + i, seg_height + 1, black); // G
    delay(anim_speed);
  }
}

void Digit::morph8() {
  // EIGHT
  for (int i = 0; i <= seg_width; i++)
  {
    // Move B right to left
    draw_line(seg_width - i, seg_height * 2 + 1, seg_width - i, seg_height + 2, colour);
    if (i > 0) draw_line(seg_width - i + 1, seg_height * 2 + 1, seg_width - i + 1, seg_height + 2, black);

    // Move C right to left
    draw_line(seg_width - i, 1, seg_width - i, seg_height, colour);
    if (i > 0) draw_line(seg_width - i + 1, 1, seg_width - i + 1, seg_height, black);

    // Gradually draw D and G
    if (i < seg_width) {
      draw_pixel(seg_width - i, 0, colour); // D
      draw_pixel(seg_width - i, seg_height + 1, colour); // G
    }
    delay(anim_speed);
  }
}

void Digit::morph9() {
  // NINE
  for (int i = 0; i <= (seg_width + 1); i++)
  {
    // Move E left to right
    draw_line(0 + i - 1, 1, 0 + i - 1, seg_height, black);
    draw_line(0 + i, 1, 0 + i, seg_height, colour);
    delay(anim_speed);
  }
}

void Digit::morph0() {
  // ZERO
  for (int i = 0; i <= seg_width; i++)
  {
    if (current_value==1) { // If 1 to 0, slide B to F and E to C  
      // slide B to F 
      draw_line(seg_width - i, seg_height * 2+1 , seg_width - i, seg_height + 2, colour);
      if (i > 0) draw_line(seg_width - i + 1, seg_height * 2+1, seg_width - i + 1, seg_height + 2, black);

      // slide E to C
      draw_line(seg_width - i, 1, seg_width - i, seg_height, colour);
      if (i > 0) draw_line(seg_width - i + 1, 1, seg_width - i + 1, seg_height, black);

      if (i<seg_width) draw_pixel(seg_width - i, seg_height * 2 + 2 , colour); // Draw A
      if (i<seg_width) draw_pixel(seg_width - i, 0, colour); // Draw D
    }
    
    if (current_value==2) { // If 2 to 0, slide B to F and Flow G to C
      // slide B to F 
      draw_line(seg_width - i, seg_height * 2+1 , seg_width - i, seg_height + 2, colour);
      if (i > 0) draw_line(seg_width - i + 1, seg_height * 2+1, seg_width - i + 1, seg_height + 2, black);
    
      draw_pixel(1+i, seg_height + 1, black); // Erase G left to right
      if (i<seg_width) draw_pixel(seg_width + 1, seg_height + 1- i, colour);// Draw C
    }

    if (current_value==3) { // B to F, C to E
      // slide B to F 
      draw_line(seg_width - i, seg_height * 2+1 , seg_width - i, seg_height + 2, colour);
      if (i > 0) draw_line(seg_width - i + 1, seg_height * 2+1, seg_width - i + 1, seg_height + 2, black);
      
      // Move C to E
      draw_line(seg_width - i, 1, seg_width - i, seg_height, colour);
      if (i > 0) draw_line(seg_width - i + 1, 1, seg_width - i + 1, seg_height, black);

      // Erase G from right to left
      draw_pixel(seg_width - i, seg_height + 1, black); // G
    }
    
    if (current_value==5) { // If 5 to 0, we also need to slide F to B
      if (i<seg_width) {
        if (i>0) draw_line(1 + i, seg_height * 2 + 1, 1 + i, seg_height + 2, black);
        draw_line(2 + i, seg_height * 2 + 1, 2 + i, seg_height + 2, colour);
      }
    }
    
    if (current_value==5 || current_value==9) { // If 9 or 5 to 0, Flow G into E
      if (i<seg_width) draw_pixel(seg_width - i, seg_height + 1, black);
      if (i<seg_width) draw_pixel(0, seg_height - i, colour);
    }
    delay(anim_speed);
  }
}

void Digit::morph1() {
  // Zero or two to One
  for (int i = 0; i <= (seg_width + 1); i++)
  {
    // Move E left to right
    draw_line(0 + i - 1, 1, 0 + i - 1, seg_height, black);
    draw_line(0 + i, 1, 0 + i, seg_height, colour);

    // Move F left to right
    draw_line(0 + i - 1, seg_height * 2 + 1, 0 + i - 1, seg_height + 2, black);
    draw_line(0 + i, seg_height * 2 + 1, 0 + i, seg_height + 2, colour);

    // Gradually Erase A, G, D
    draw_pixel(1 + i, seg_height * 2 + 2, black); // A
    draw_pixel(1 + i, 0, black); // D
    draw_pixel(1 + i, seg_height + 1, black); // G

    delay(anim_speed);
  }
}

void Digit::morph(byte new_value) {
  switch (new_value) {
    case 2: morph2(); break;
    case 3: morph3(); break;
    case 4: morph4(); break;
    case 5: morph5(); break;
    case 6: morph6(); break;
    case 7: morph7(); break;
    case 8: morph8(); break;
    case 9: morph9(); break;
    case 0: morph0(); break;
    case 1: morph1(); break;
  }
  current_value = new_value;
}

