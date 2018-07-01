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

// Pins for LED MATRIX
PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  //display.displayTestPattern(70);
  display.display(70);
}
#endif

#ifdef ESP32
void IRAM_ATTR display_updater() {
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  //isplay.display(70);
  display.displayTestPattern(70);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif

//=== SEGMENTS ===
#include "Digit.h"
Digit digit0(&display, 0, 63 - 9, 8, display.color565(0, 0, 255));
Digit digit1(&display, 0, 63 - 9*2, 8, display.color565(0, 0, 255));
Digit digit2(&display, 0, 63 - 3 - 9*3, 8, display.color565(0, 0, 255));
Digit digit3(&display, 0, 63 - 3 - 9*4, 8, display.color565(0, 0, 255));
Digit digit4(&display, 0, 63 - 6 - 9*5, 8, display.color565(0, 0, 255));
Digit digit5(&display, 0, 63 - 6 - 9*6, 8, display.color565(0, 0, 255));
int changeSpeed = 500;

//=== CLOCK ===
#include "NTPClient.h"
NTPClient ntpClient;
unsigned long lastEpoch;
byte prevhh;
byte prevmm;
byte prevss;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  display.begin(16);

#ifdef ESP8266
  display_ticker.attach(0.002, display_updater);
#endif

#ifdef ESP32
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &display_updater, true);
  timerAlarmWrite(timer, 2000, true);
  timerAlarmEnable(timer);
#endif

  ntpClient.Setup();

  display.fillScreen(display.color565(0, 0, 0));
  digit1.DrawColon(display.color565(0, 0, 31));
  digit3.DrawColon(display.color565(0, 0, 31));
//  digit0.Draw(0);
//  digit1.Draw(0);
//  digit2.Draw(0);
//  digit3.Draw(0);
//  digit4.Draw(0);
//  digit5.Draw(0);
  delay(changeSpeed);
}


void loop() {
  unsigned long epoch = ntpClient.GetCurrentTime();
  //Serial.print("GetCurrentTime returned epoch = ");
  //Serial.println(epoch);
  if (epoch != 0) ntpClient.PrintTime();

  if (epoch != lastEpoch) {
    int hh = ntpClient.GetHours();
    int mm = ntpClient.GetMinutes();
    int ss = ntpClient.GetSeconds();
    if (lastEpoch == 0) { // If we didn't have a previous time. Just draw it without morphing.
      digit0.Draw(ss % 10);
      digit1.Draw(ss / 10);
      digit2.Draw(mm % 10);
      digit3.Draw(mm / 10);
      digit4.Draw(hh % 10);
      digit5.Draw(hh / 10);
    }
    else
    {
      int s0 = ss % 10;
      int s1 = ss / 10;
      if (s0!=digit0.Value()) digit0.Morph(s0);
      if (s1!=digit1.Value()) digit1.Morph(s1);

      int m0 = mm % 10;
      int m1 = mm / 10;
      if (m0!=digit2.Value()) digit2.Morph(m0);
      if (m1!=digit3.Value()) digit3.Morph(m1);
      
      int h0 = hh % 10;
      int h1 = hh / 10;
      if (h0!=digit4.Value()) digit4.Morph(h0);
      if (h1!=digit5.Value()) digit5.Morph(h1);
    }
    lastEpoch = epoch;
  }

  //delay(1000);
}
