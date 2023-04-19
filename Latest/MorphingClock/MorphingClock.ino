// Morphing Clock by Hari Wiguna, July 2018
//
// Thanks to:
// Dominic Buchstaller for PxMatrix
// Tzapu for WifiManager
// Stephen Denne aka Datacute for DoubleResetDetector
// Brian Lough aka WitnessMeNow for tutorials on the matrix and WifiManager

//#define double_buffer
#include <PxMatrix.h>


#include "NTPClient.h"
#include "Digit.h"
#include "ClockDisplay.h"

//Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

// Pins for LED MATRIX
PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

ClockDisplay clockDisplay;

//=== CLOCK ===
NTPClient ntpClient;
unsigned long prevEpoch;
byte prevhh;
byte prevmm;
byte prevss;

void setup() {
  Serial.begin(9600);

  clockDisplay = ClockDisplay(&display);

  ntpClient.Setup(&clockDisplay);
}


void loop() {
  unsigned long epoch = ntpClient.GetCurrentTime();
  
  if (epoch != 0) ntpClient.PrintTime();

  if (epoch != prevEpoch) {
    int hh = ntpClient.GetHours();
    int mm = ntpClient.GetMinutes();
    int ss = ntpClient.GetSeconds();
    if (prevEpoch == 0) { // If we didn't have a previous time. Just draw it without morphing.
      clockDisplay.showTime(hh, mm, ss);
    }
    else
    {
      // epoch changes every miliseconds, we only want to draw when digits actually change.
      clockDisplay.morphTime(hh, mm, ss);
    }
    prevEpoch = epoch;
  }
}
