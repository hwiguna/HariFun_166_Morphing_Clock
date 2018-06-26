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
Digit digit0(&display, 0, 28 + 9, 8, display.color565(0, 0, 255));
Digit digit1(&display, 0, 28, 8, display.color565(0, 0, 255));
Digit digit2(&display, 0, 28 - 9, 8, display.color565(0, 0, 255));
Digit digit3(&display, 0, 28 - 9 - 9, 8, display.color565(0, 0, 255));
int changeSpeed = 500;

//=== CLOCK ===
#include "NTPClient.h"
NTPClient ntpClient;
unsigned long lastEpoch;

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
  digit0.Draw(0);
  digit1.Draw(0);
  digit2.Draw(0);
  digit3.Draw(0);
  delay(changeSpeed);
}


void loop() {
  unsigned long epoch = ntpClient.GetCurrentEpoch();
  Serial.print("GetCurrentEpoch returned epoch = ");
  Serial.println(epoch);
  if (epoch != 0) ntpClient.PrintTime();

  if (epoch != lastEpoch) {
    byte hh = ntpClient.GetHours();
    byte mm = ntpClient.GetMinutes();
    byte ss = ntpClient.GetSeconds();
    if (lastEpoch == 0) { // If we didn't have a previous time. Just draw it without morphing.
      digit0.Draw(ss % 10);
      digit1.Draw(ss / 10);
      digit2.Draw(mm % 10);
      digit3.Draw(mm / 10);
    }
    else
    {
      for (byte d = 1; d <= 10; d++) {
        if (d == 10) {
          digit0.Morph(0);
          int nextTens = digit1.Value() + 1;
          digit1.Morph(nextTens == 10 ? 0 : nextTens);
        }
        else
          digit0.Morph(d);
        delay(changeSpeed);
      }
    }
    lastEpoch = epoch;
  }

  // delay(1000);
}
