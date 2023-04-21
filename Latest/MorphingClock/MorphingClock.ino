// Morphing Clock by Hari Wiguna, July 2018
//
// Thanks to:
// Dominic Buchstaller for PxMatrix
// Tzapu for WifiManager
// Stephen Denne aka Datacute for DoubleResetDetector
// Brian Lough aka WitnessMeNow for tutorials on the matrix and WifiManager

#include "NTPClient.h"
#include "Digit.h"
#include "ClockDisplay.h"

ClockDisplay clockDisplay;

//=== CLOCK ===
NTPClient ntpClient;
unsigned long prevEpoch;
byte prevhh;
byte prevmm;
byte prevss;

void setup() {
  Serial.begin(9600);

  clockDisplay = ClockDisplay();

  ntpClient.Setup(&clockDisplay);
}

void loop() {
  unsigned long epoch = ntpClient.GetCurrentTime();
  
  if (epoch != 0) ntpClient.PrintTime();

  if (epoch != prevEpoch) {
    int hh = ntpClient.GetHours();
    int mm = ntpClient.GetMinutes();
    int ss = ntpClient.GetSeconds();
    bool isPM = ntpClient.GetIsPM();
    bool military = ntpClient.GetIsMilitary();

    if (prevEpoch == 0) { // If we didn't have a previous time. Just draw it without morphing.
      clockDisplay.showTime(hh, mm, ss, isPM, military);//Need to not hard code the 'false' here
    }
    else
    {
      // epoch changes every miliseconds, we only want to draw when digits actually change.
      clockDisplay.morphTime(hh, mm, ss, isPM, military);
    }
    prevEpoch = epoch;
  }
}
