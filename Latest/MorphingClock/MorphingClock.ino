// Morphing Clock - Another Remix
//_________________________________________________________________
// **Thanks to:**
// Harifun for his original 6 digit morphing clock
// lmirel for their remix of the clock
// timz3818's remix of the clock
// Dominic Buchstaller for PxMatrix
// Tzapu for WifiManager
// Stephen Denne aka Datacute for DoubleResetDetector
// Brian Lough aka WitnessMeNow for tutorials on the matrix and WifiManager

#include "NTPClient.h"
#include "Digit.h"
#include "ClockDisplay.h"

ClockDisplay clockDisplay;

NTPClient ntpClient;
unsigned long prevEpoch;

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
