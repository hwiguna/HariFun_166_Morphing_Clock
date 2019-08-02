// Morphing Clock improved by SnowHead, July 2019
//
// Thanks to:
// Hari Wiguna for the primary solution
// Dominic Buchstaller for PxMatrix
// Tzapu for WifiManager
// Stephen Denne aka Datacute for DoubleResetDetector
// Brian Lough aka WitnessMeNow for tutorials on the matrix and WifiManager

#define double_buffer
#include <PxMatrix.h>

#ifdef ESP32

#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#endif

#ifdef ESP8266

#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2
#endif

// digit color (for ESP8266 only pure red, green or blue will work, mixed colors will flicker)
#define COL_ACT   display.color565(0, 255, 0)  // green

// Pins for LED MATRIX 1/16
PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);

// Pins for LED MATRIX 1/32
//PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);


// animation delay in ms
int animSpeed = 30;

#ifdef ESP8266
  #define refreshRate 0.020 // higher allows more time for WiFi, but makes the display dimmer. Originally 0.002
  #define persistenceMicroSeconds 200 // Higher = brighter.  Originally 70
#else
  #define refreshRate 0.002 // higher allows more time for WiFi, but makes the display dimmer. Originally 0.002
  #define persistenceMicroSeconds 70 // Higher = brighter.  Originally 70
#endif

#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  //display.displayTestPattern(70);
  display.display(persistenceMicroSeconds); // How many microseconds to enable the display
}
#endif

//#ifdef ESP32
//void IRAM_ATTR display_updater() {
//  // Increment the counter and set the time of ISR
//  portENTER_CRITICAL_ISR(&timerMux);
//  //display.display(70);
//  display.displayTestPattern(70);
//  portEXIT_CRITICAL_ISR(&timerMux);
//}
//#endif

//=== SEGMENTS ===
#include "Digit.h"
Digit digit0(&display, 0, 63 - 1 - 9*1, 8, COL_ACT);
Digit digit1(&display, 0, 63 - 1 - 9*2, 8, COL_ACT);
Digit digit2(&display, 0, 63 - 4 - 9*3, 8, COL_ACT);
Digit digit3(&display, 0, 63 - 4 - 9*4, 8, COL_ACT);
Digit digit4(&display, 0, 63 - 7 - 9*5, 8, COL_ACT);
Digit digit5(&display, 0, 63 - 7 - 9*6, 8, COL_ACT);

//=== CLOCK ===
#include "WTAClient.h" 
WTAClient wtaClient;
unsigned long prevEpoch;
byte prevhh;
byte prevmm;
byte prevss;


void setup() {
  Serial.begin(115200);
  display.begin(16);

#ifdef ESP8266
  //display_ticker.attach(0.002, display_updater);
  // First parameter is how often the display should be refreshed.
  // Originally 0.002 caused unreliable WiFi on some NodeMCU.
  // Hari changed this to 0.02 so it refresh less frequently.  
  // This caused display to be dimmer, so I increased how long the display should be latched from 70 to 500
  display_ticker.attach(refreshRate, display_updater);
#endif

//#ifdef ESP32
//  timer = timerBegin(0, 80, true);
//  timerAttachInterrupt(timer, &display_updater, true);
//  timerAlarmWrite(timer, 2000, true);
//  timerAlarmEnable(timer);
//#endif

  wtaClient.Setup(&display);

  display.fillScreen(display.color565(0, 0, 0));
  digit1.DrawColon(COL_ACT);
  digit3.DrawColon(COL_ACT);
}


void loop() {
  unsigned long epoch = wtaClient.GetCurrentTime();
  //Serial.print("GetCurrentTime returned epoch = ");
  //Serial.println(epoch);
  if (epoch != 0) wtaClient.PrintTime();

  if (epoch != prevEpoch) {
    int hh = wtaClient.GetHours();
    int mm = wtaClient.GetMinutes();
    int ss = wtaClient.GetSeconds();
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
        digit0.SetValue(s0);
        digit1.SetValue(s1);
        prevss = ss;
      }

      if (mm!=prevmm) {
        int m0 = mm % 10;
        int m1 = mm / 10;
        digit2.SetValue(m0);
        digit3.SetValue(m1);
        prevmm = mm;
      }
      
      if (hh!=prevhh) {
        int h0 = hh % 10;
        int h1 = hh / 10;
        digit4.SetValue(h0);
        digit5.SetValue(h1);
        prevhh = hh;
      }
    }
    prevEpoch = epoch;
  }
    digit0.Morph();
    digit1.Morph();
    digit2.Morph();
    digit3.Morph();
    digit4.Morph();
    digit5.Morph();
    delay(animSpeed);
}
