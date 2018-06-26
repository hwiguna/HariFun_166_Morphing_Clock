#include "Digit.h"

const byte sA = 0;
const byte sB = 1;
const byte sC = 2;
const byte sD = 3;
const byte sE = 4;
const byte sF = 5;
const byte sG = 6;
const int segHeight = 6;
const int segWidth = segHeight;
const uint16_t height = 31;
const uint16_t width = 63;

uint16_t black;

Digit::Digit(PxMATRIX* d, byte value, uint16_t xo, uint16_t yo, uint16_t color) {
  _display = d;
  _value = value;
  xOffset = xo;
  yOffset = yo;
  _color = color;
}

byte Digit::Value() {
  return _value;
}
void Digit::drawPixel(uint16_t x, uint16_t y, uint16_t c)
{
  _display->drawPixel(xOffset + x, height - (y + yOffset), c);
}

void Digit::drawLine(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, uint16_t c)
{
  _display->drawLine(xOffset + x, height - (y + yOffset), xOffset + x2, height - (y2 + yOffset), c);
}

void Digit::drawSeg(byte seg)
{
  switch (seg) {
    case sA: drawLine(1, segHeight * 2 + 2, segWidth, segHeight * 2 + 2, _color); break;
    case sB: drawLine(segWidth + 1, segHeight * 2 + 1, segWidth + 1, segHeight + 2, _color); break;
    case sC: drawLine(segWidth + 1, 1, segWidth + 1, segHeight, _color); break;
    case sD: drawLine(1, 0, segWidth, 0, _color); break;
    case sE: drawLine(0, 1, 0, segHeight, _color); break;
    case sF: drawLine(0, segHeight * 2 + 1, 0, segHeight + 2, _color); break;
    case sG: drawLine(1, segHeight + 1, segWidth, segHeight + 1, _color); break;
  }
}

void Digit::Draw(byte value) {
  switch (value) {
    case 0: // ZERO
      drawSeg(sA);
      drawSeg(sB);
      drawSeg(sC);
      drawSeg(sD);
      drawSeg(sE);
      drawSeg(sF);
      break;
    case 1: // ONE
      drawSeg(sB);
      drawSeg(sC);
      break;
      case 2: // TWO
      drawSeg(sA);
      drawSeg(sB);
      drawSeg(sD);
      drawSeg(sE);
      drawSeg(sG);
      break;
      case 4: // FOUR
      drawSeg(sB);
      drawSeg(sC);
      drawSeg(sF);
      drawSeg(sG);
      break;
  }
  _value = value;
}

void Digit::Morph2() {
  // TWO
  for (int i = 0; i <= segWidth; i++)
  {
    if (i < segWidth) {
      drawPixel(segWidth - i, segHeight * 2 + 2, _color);
      drawPixel(segWidth - i, segHeight + 1, _color);
      drawPixel(segWidth - i, 0, _color);
    }

    drawLine(segWidth + 1 - i, 1, segWidth + 1 - i, segHeight, black);
    drawLine(segWidth - i, 1, segWidth - i, segHeight, _color);
    delay(animSpeed);
  }
}

void Digit::Morph3() {
  // THREE
  for (int i = 0; i <= segWidth; i++)
  {
    drawLine(0 + i, 1, 0 + i, segHeight, black);
    drawLine(1 + i, 1, 1 + i, segHeight, _color);
    delay(animSpeed);
  }
}

void Digit::Morph4() {
  // FOUR
  for (int i = 0; i < segWidth; i++)
  {
    drawPixel(segWidth - i, segHeight * 2 + 2, black); // Erase A
    drawPixel(0, segHeight * 2 + 1 - i, _color); // Draw as F
    drawPixel(1 + i, 0, black); // Erase D
    delay(animSpeed);
  }
}

void Digit::Morph5() {
  // FIVE
  for (int i = 0; i < segWidth; i++)
  {
    drawPixel(segWidth + 1, segHeight + 2 + i, black); // Erase B
    drawPixel(segWidth - i, segHeight * 2 + 2, _color); // Draw as A
    drawPixel(segWidth - i, 0, _color); // Draw D
    delay(animSpeed);
  }
}

void Digit::Morph6() {
  // SIX
  for (int i = 0; i <= segWidth; i++)
  {
    // Move C right to left
    drawLine(segWidth - i, 1, segWidth - i, segHeight, _color);
    if (i > 0) drawLine(segWidth - i + 1, 1, segWidth - i + 1, segHeight, black);
    delay(animSpeed);
  }
}

void Digit::Morph7() {
  // SEVEN
  for (int i = 0; i <= (segWidth + 1); i++)
  {
    // Move E left to right
    drawLine(0 + i - 1, 1, 0 + i - 1, segHeight, black);
    drawLine(0 + i, 1, 0 + i, segHeight, _color);

    // Move F left to right
    drawLine(0 + i - 1, segHeight * 2 + 1, 0 + i - 1, segHeight + 2, black);
    drawLine(0 + i, segHeight * 2 + 1, 0 + i, segHeight + 2, _color);

    // Erase D and G gradually
    drawPixel(1 + i, 0, black); // D
    drawPixel(1 + i, segHeight + 1, black); // G
    delay(animSpeed);
  }
}

void Digit::Morph8() {
  // EIGHT
  for (int i = 0; i <= segWidth; i++)
  {
    // Move B right to left
    drawLine(segWidth - i, segHeight * 2 + 1, segWidth - i, segHeight + 2, _color);
    if (i > 0) drawLine(segWidth - i + 1, segHeight * 2 + 1, segWidth - i + 1, segHeight + 2, black);

    // Move C right to left
    drawLine(segWidth - i, 1, segWidth - i, segHeight, _color);
    if (i > 0) drawLine(segWidth - i + 1, 1, segWidth - i + 1, segHeight, black);

    // Gradually draw D and G
    if (i < segWidth) {
      drawPixel(segWidth - i, 0, _color); // D
      drawPixel(segWidth - i, segHeight + 1, _color); // G
    }
    delay(animSpeed);
  }
}

void Digit::Morph9() {
  // NINE
  for (int i = 0; i <= (segWidth + 1); i++)
  {
    // Move E left to right
    drawLine(0 + i - 1, 1, 0 + i - 1, segHeight, black);
    drawLine(0 + i, 1, 0 + i, segHeight, _color);
    delay(animSpeed);
  }
}

void Digit::Morph0() {
  // ZERO
  for (int i = 0; i < segWidth; i++)
  {
    // Flow G into E
    drawPixel(segWidth - i, segHeight + 1, black);
    drawPixel(0, segHeight - i, _color);
    delay(animSpeed);
  }
}

void Digit::Morph1() {
  // Zero to One
  for (int i = 0; i <= (segWidth + 1); i++)
  {
    // Move E left to right
    drawLine(0 + i - 1, 1, 0 + i - 1, segHeight, black);
    drawLine(0 + i, 1, 0 + i, segHeight, _color);

    // Move F left to right
    drawLine(0 + i - 1, segHeight * 2 + 1, 0 + i - 1, segHeight + 2, black);
    drawLine(0 + i, segHeight * 2 + 1, 0 + i, segHeight + 2, _color);

    // Gradually Erase A, G, D
    drawPixel(1 + i, segHeight * 2 + 2, black); // A
    drawPixel(1 + i, 0, black); // D
    drawPixel(1 + i, segHeight + 1, black); // G

    delay(animSpeed);
  }
}

void Digit::Morph(byte newValue) {
  switch (newValue) {
    case 2: Morph2(); break;
    case 3: Morph3(); break;
    case 4: Morph4(); break;
    case 5: Morph5(); break;
    case 6: Morph6(); break;
    case 7: Morph7(); break;
    case 8: Morph8(); break;
    case 9: Morph9(); break;
    case 0: Morph0(); break;
    case 1: Morph1(); break;
  }
  _value = newValue;
}

