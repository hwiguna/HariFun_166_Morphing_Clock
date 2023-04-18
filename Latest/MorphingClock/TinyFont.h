/*
 * mirel.lazar@gmail.com
 * 
 * provided 'AS IS', use it at your own risk
 */
#ifndef TINYFONT_H
#define TINYFONT_H

#include <Arduino.h>
#include <PxMatrix.h> // https://github.com/2dom/PxMatrix

#define TF_COLS 4
#define TF_ROWS 5
struct TFFace 
{
  char fface[TF_ROWS]; //4 cols x 5 rows
};
/*
 * example:
  yo = 1;
  int cc = random (25, 65535);
  xo  = 0; TFDrawChar (&display, 'L', xo, yo, cc); cc = random (25, 65535);
  xo += 5; TFDrawChar (&display, 'O', xo, yo, cc); cc = random (25, 65535);
  xo += 5; TFDrawChar (&display, 'V', xo, yo, cc); cc = random (25, 65535);
  xo += 5; TFDrawChar (&display, 'E', xo, yo, cc); cc = random (25, 65535);
 */
void TFDrawChar (PxMATRIX* d, char value, char xo, char yo, int col);

/*
 * example:
  String lstr = String (tempM) + "C";
  int cc_red = display.color565 (255, 0, 0);
  xo = 1; yo = 1;
  TFDrawText (&display, lstr, xo, yo, cc_red);
 */
void TFDrawText (PxMATRIX* d, String text, char xo, char yo, int col);

#endif

