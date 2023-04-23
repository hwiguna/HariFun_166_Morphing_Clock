/*
 * mirel.lazar@gmail.com
 * 
 * provided 'AS IS', use it at your own risk
 */
#include "TinyIcons.h"

#define DEBUG 0
#define debug_println(...) \
            do { if (DEBUG) Serial.println(__VA_ARGS__); } while (0)
#define debug_print(...) \
            do { if (DEBUG) Serial.print(__VA_ARGS__); } while (0)

void draw_icon(PxMATRIX* d, int *ico, char xo, char yo, int cols, int rows)
{
  debug_print("draw icon ");
  debug_print(cols);
  debug_print(" x ");
  debug_print(rows);
  debug_print(": ");
  int i, j;
  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < cols; j++)
    {
      d->drawPixel(xo + j, yo + i, ico[i * cols + j]);
      debug_print(ico[i * cols + j]);
      debug_print(", ");
    }
  }
  debug_println ();
}

