// Morphing Clock by Hari Wiguna, July 2018
//
// Thanks to:
// Dominic Buchstaller for PxMatrix
// Tzapu for WifiManager
// Stephen Denne aka Datacute for DoubleResetDetector
// Brian Lough aka WitnessMeNow for tutorials on the matrix and WifiManager

//#define double_buffer
#include <PxMatrix.h>
#include <Ticker.h>

#include "NTPClient.h"
#include "Digit.h"

Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

// Pins for LED MATRIX
PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

// ISR for display refresh
void display_updater()
{
  display.display(70);
}

//=== SEGMENTS ===
Digit digit0(&display, 0, 63 - 1 - 9*1, 8, display.color565(0, 0, 255));
Digit digit1(&display, 0, 63 - 1 - 9*2, 8, display.color565(0, 0, 255));
Digit digit2(&display, 0, 63 - 4 - 9*3, 8, display.color565(0, 0, 255));
Digit digit3(&display, 0, 63 - 4 - 9*4, 8, display.color565(0, 0, 255));
Digit digit4(&display, 0, 63 - 7 - 9*5, 8, display.color565(0, 0, 255));
Digit digit5(&display, 0, 63 - 7 - 9*6, 8, display.color565(0, 0, 255));

//=== CLOCK ===
NTPClient ntpClient;
unsigned long prevEpoch;
byte prevhh;
byte prevmm;
byte prevss;


void setup() {
  Serial.begin(9600);
  display.begin(16);

  Serial.println("setting one pixel");
  display.drawPixel(1, 1, display.color565(0, 255, 255));

  display_ticker.attach(0.002, display_updater);

  ntpClient.Setup(&display);

  display.fillScreen(display.color565(0, 0, 0));
  digit1.DrawColon(display.color565(0, 0, 255));
  digit3.DrawColon(display.color565(0, 0, 255));
}


void loop() {
  unsigned long epoch = ntpClient.GetCurrentTime();
  
  if (epoch != 0) ntpClient.PrintTime();

  if (epoch != prevEpoch) {
    int hh = ntpClient.GetHours();
    int mm = ntpClient.GetMinutes();
    int ss = ntpClient.GetSeconds();
    if (prevEpoch == 0) { // If we didn't have a previous time. Just draw it without morphing.
      digit0.Draw(ss % 10);
      digit1.Draw(ss / 10);
      digit2.Draw(mm % 10);
      digit3.Draw(mm / 10);
      digit4.Draw(hh % 10);
      digit5.Draw(hh / 10);
    }
    else
    {
      // epoch changes every miliseconds, we only want to draw when digits actually change.
      if (ss!=prevss) { 
        int s0 = ss % 10;
        int s1 = ss / 10;
        if (s0!=digit0.Value()) digit0.Morph(s0);
        if (s1!=digit1.Value()) digit1.Morph(s1);
        
        prevss = ss;
      }

      if (mm!=prevmm) {
        int m0 = mm % 10;
        int m1 = mm / 10;
        if (m0!=digit2.Value()) digit2.Morph(m0);
        if (m1!=digit3.Value()) digit3.Morph(m1);
        prevmm = mm;
      }
      
      if (hh!=prevhh) {
        int h0 = hh % 10;
        int h1 = hh / 10;
        if (h0!=digit4.Value()) digit4.Morph(h0);
        if (h1!=digit5.Value()) digit5.Morph(h1);
        prevhh = hh;
      }
    }
    prevEpoch = epoch;
  }
}
