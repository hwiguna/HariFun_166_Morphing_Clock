// Morphing Clock improved by SnowHead, July 2019
//
// Thanks to:
// Hari Wiguna for the primary solution
// Dominic Buchstaller for PxMatrix
// Tzapu for WifiManager
// Stephen Denne aka Datacute for DoubleResetDetector
// Brian Lough aka WitnessMeNow for tutorials on the matrix and WifiManager

#include "errno.h"
#include "WTAClient.h"
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

// Pins for LED MATRIX 1/16
PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);

// digit color (for ESP8266 only pure red, green or blue will work, dimmed colors will flicker)
uint16_t COL_ACT = display.color565(0, 255, 0);  // green

// Pins for LED MATRIX 1/32
//PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

// the web-server-object
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)

// structure holds the parameters to save nonvolatile memory
#include <EEPROM.h>
#define EEPROM_SIZE 128        // size of NV-memory
typedef struct
{
  unsigned char h24;
  unsigned char fade;
  unsigned char r, g, b;
  unsigned char brightness;
  unsigned char timezone[32];
  unsigned char nm_start;
  unsigned char nm_end;
  unsigned char nm_brightness;
  unsigned char valid;
} mclock_struct;
mclock_struct mclock;
mclock_struct *mclockp = &mclock;
unsigned char *eptr;
uint16_t brightness;
bool nightmode;

char outstr[512] = "";
// string to hold the last displayed word-sequence
char tstr1[128], tstr2[128] = "";

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
enum
{
  DIG_S0, DIG_S1, DIG_M0, DIG_M1, DIG_H0, DIG_H1, NUM_DIGITS
};

Digit digit0(&display, 0, 63 - 1 - 9 * 1, 8, COL_ACT);
Digit digit1(&display, 0, 63 - 1 - 9 * 2, 8, COL_ACT);
Digit digit2(&display, 0, 63 - 4 - 9 * 3, 8, COL_ACT);
Digit digit3(&display, 0, 63 - 4 - 9 * 4, 8, COL_ACT);
Digit digit4(&display, 0, 63 - 7 - 9 * 5, 8, COL_ACT);
Digit digit5(&display, 0, 63 - 7 - 9 * 6, 8, COL_ACT);
Digit *Digits[NUM_DIGITS] =
{ &digit0, &digit1, &digit2, &digit3, &digit4, &digit5 };

//=== CLOCK ===
#include "WTAClient.h"
WTAClient wtaClient;
volatile unsigned long prevEpoch;
byte prevhh;
byte prevmm;
byte prevss;

void update_color(void)
{
  COL_ACT = display.color565((brightness * mclockp->r) / 40, (brightness * mclockp->g) / 40, (brightness * mclockp->b) / 40);
  for (int i = 0; i < NUM_DIGITS; i++)
    Digits[i]->SetColor(COL_ACT);
  display.fillScreen(display.color565(0, 0, 0));
  Digits[DIG_S1]->DrawColon(COL_ACT);
  Digits[DIG_M1]->DrawColon(COL_ACT);
  prevEpoch = 0;
}

// dynamic generation of the web-servers response
void page_out(void)
{
  if (strlen(outstr))
  {
    server.sendContent(outstr);
    *outstr = 0;
  }
  else
  {
    server.sendContent("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"><html><head><meta http-equiv=\"content-type\"content=\"text/html; charset=ISO-8859-1\"><title>MorphClock_Server</title></head>\r\n<body><h1 style=\"text-align: center; width: 504px;\" align=\"left\"><span style=\"color: rgb(0, 0, 153); font-weight: bold;\">MorphingClock-Server</span></h1><h3 style=\"text-align: center; width: 504px;\" align=\"left\"><span style=\"color: rgb(0, 0, 0); font-weight: bold;\">");
    server.sendContent(tstr2);
    server.sendContent("</span></h3><form method=\"get\"><table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" height=\"144\" width=\"487\"><tbody><tr><td width=\"120\"><b><big> Hour mode <td width=\"50\"></td><td><input name=\"HMOD24\" type=\"checkbox\" mode=\"submit\"");
    if (mclockp->h24)
      server.sendContent(" checked");
    server.sendContent("> 24h </td></tr>");
    server.sendContent("<tr><td><b><big>Timezone</big></b></td><td></td><td><input maxlength=\"31\" size=\"31\" name=\"TIMEZONE\" value=\"");
    server.sendContent((char*)&mclockp->timezone);
    server.sendContent("\"></td></tr><tr><td> </td><td> </td><td>See <a href=\"http://www.worldtimeapi.org/api/timezones.txt\">List</a> (\"ip\" means \"automatic\")</td></tr><tr><td> <br></td></tr>");

#ifdef ESP32
    server.sendContent("<tr><td><b><big>Brightness</big></b></td><td></td><td><input maxlength=\"3\" size=\"3\" name=\"BRIGHT\" value=\"");
    server.sendContent(String(mclockp->brightness));
    server.sendContent("\"> %</td></tr>");
#endif
    server.sendContent("<tr><td><b><big>Nightmode</font></big></b><td></td>");
#ifdef ESP32
    server.sendContent("<td>Brightness<br><input maxlength=\"3\" size=\"3\" name=\"NMBRIGHT\" value=\"");
    server.sendContent(String(mclockp->nm_brightness));
    server.sendContent("\"> %</td>");
#endif
    server.sendContent("<td>Start<br><input maxlength=\"3\" size=\"3\" name=\"NMSTART\" value=\"");
    server.sendContent(((mclockp->nm_start > 9) ? "" : "0") + String(mclockp->nm_start));
    server.sendContent("\"> h</td><td>End<br><input maxlength=\"3\" size=\"3\" name=\"NMEND\" value=\"");
    server.sendContent(((mclockp->nm_end > 9) ? "" : "0") + String(mclockp->nm_end));
    server.sendContent("\"> h</td></tr><tr><td> <br></tr>");
    server.sendContent("</td></tr><tr><td><b><big>Fading</font></big></b></td><td></td><td><input maxlength=\"3\" size=\"3\" name=\"FADE\" value=\"");
    server.sendContent(String(mclockp->fade));
    server.sendContent("\"> ms</td></tr>");
#ifdef ESP32
    server.sendContent("<tr><td><b><big><font color=\"#cc0000\">Red</font></big></b></td><td></td><td><input maxlength=\"3\" size=\"3\" name=\"RED\" value=\"");
    server.sendContent(String(mclockp->r));
    server.sendContent("\"> %</td></tr><tr><td><b><big><font color=\"#006600\">Green</font></big></b></td><td></td><td><input maxlength=\"3\" size=\"3\" name=\"GREEN\" value=\"");
    server.sendContent(String(mclockp->g));
    server.sendContent("\"> %</td></tr><tr><td><b><big><font color=\"#000099\">Blue</font></big></b></td><td></td><td><input maxlength=\"3\" size=\"3\" name=\"BLUE\" value=\"");
    server.sendContent(String(mclockp->b));
    server.sendContent("\"> %</td></tr>");
#else
    server.sendContent("<tr><td width=\"120\"><b><big><font color=\"#cc0000\">Red</font></big></b></td><td></td><td><input name=\"RED\" type=\"checkbox\" mode=\"submit\"");
    if (mclockp->r)
      server.sendContent(" checked");
    server.sendContent("></td></tr>");
    server.sendContent("<tr><td width=\"120\"><b><big><font color=\"#006600\">Green</font></big></b></td><td></td><td><input name=\"GREEN\" type=\"checkbox\" mode=\"submit\"");
    if (mclockp->g)
      server.sendContent(" checked");
    server.sendContent("></td></tr>");
    server.sendContent("<tr><td width=\"120\"><b><big><font color=\"#000099\">Blue</font></big></b></td><td></td><td><input name=\"BLUE\" type=\"checkbox\" mode=\"submit\"");
    if (mclockp->b)
      server.sendContent(" checked");
    server.sendContent("></td></tr>");
#endif
    server.sendContent("</tbody></table><br><table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" height=\"30\" width=\"99\"><tbody><tr><td><input name=\"SEND\" value=\" Send \" type=\"submit\"></td></tr></tbody></table></form><table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" height=\"30\" width=\"99\"><tbody><tr><td><form method=\"get\"><input name=\"SAVE\" value=\" Save \" type=\"submit\"></form></td></tr></tbody></table></body></html>\r\n");
  }
}

void setup()
{
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

  // read stored parameters
  EEPROM.begin(EEPROM_SIZE);
  eptr = (unsigned char*) mclockp;
  for (int i = 0; i < sizeof(mclock_struct); i++)
    *(eptr++) = EEPROM.read(i);

  // EEPROM-parameters invalid, use default-values and store them
  if (mclockp->valid != 0xA5)
  {
    mclockp->h24 = 1;
    mclockp->fade = 30;
    mclockp->r = 0;
    mclockp->g = 100;
    mclockp->b = 0;
    strcpy((char*)&mclockp->timezone, "ip");
    mclockp->brightness = 100;
    mclockp->nm_start = 0;
    mclockp->nm_end = 0;
#ifdef ESP32
    mclockp->nm_brightness = 50;
#else
    mclockp->nm_brightness = 0;
#endif
    mclockp->valid = 0xA5;

    eptr = (unsigned char*) mclockp;
    for (int i = 0; i < sizeof(mclock_struct); i++)
      EEPROM.write(i, *(eptr++));
    EEPROM.commit();
  }

  brightness = mclockp->brightness;
  strcpy(timezone, (char*)&mclockp->timezone);
  military = mclockp->h24;

  wtaClient.Setup(&display);
  display.fillScreen(display.color565(0, 0, 0));

  update_color();

  // parsing function for the commands of the web-server
  server.on("/", []()
  {
    int i;
    unsigned int j;
    long pval;

    if (server.args())
    {
      for (i = 0; i < server.args(); i++)
      {
        if (server.argName(i) == "SEND")
        {
          *outstr = 0;
          mclockp->h24 = 0;
          military = 0;
#ifndef ESP32
          mclockp->r = 0;
          mclockp->g = 0;
          mclockp->b = 0;
#endif
        }
      }
      for (i = 0; i < server.args(); i++)
      {
        errno = 0;
        if (server.argName(i) == "SAVE")
        {
          unsigned char *eptr = (unsigned char*)mclockp;

          for (j = 0; j < sizeof(mclock_struct); j++)
            EEPROM.write(j, *(eptr++));
          EEPROM.commit();
          if (!server.arg(i).length())
            sprintf(outstr + strlen(outstr), "OK");
        }
        else if (server.argName(i) == "HMOD24")
        {
          if (server.arg(i) == "on")
            mclockp->h24 = 1;
          else if (server.arg(i) == "off")
            mclockp->h24 = 0;
          else
            sprintf(outstr + strlen(outstr), "HMOD24=%s\r\n", (mclockp->h24) ? "on" : "off");
          prevEpoch = 0;
          if (military != mclockp->h24)
          {
            military = mclockp->h24;
            wtaClient.GetCurrentTime(true);
          }
        }
        else if (server.argName(i) == "TIMEZONE")
        {
          if (server.arg(i).length() && !errno)
          {
            strcpy((char*)&mclockp->timezone, server.arg(i).c_str());
            strcpy((char*)&timezone, (char*)&mclockp->timezone);
            wtaClient.GetCurrentTime(true);
          }
          else
            sprintf(outstr + strlen(outstr), "TIMEZONE=%s\r\n", mclockp->timezone);
        }
        else if (server.argName(i) == "FADE")
        {
          pval = strtol(server.arg(i).c_str(), NULL, 10);
          if (server.arg(i).length() && !errno)
          {
            mclockp->fade = pval;
            if (mclockp->fade > 120)
              mclockp->fade = 120;
            if (mclockp->fade < 1)
              mclockp->fade = 1;
          }
          else
            sprintf(outstr + strlen(outstr), "FADE=%d\r\n", mclockp->fade);
        }
#ifdef ESP32
        else if (server.argName(i) == "BRIGHT")
        {
          pval = strtol(server.arg(i).c_str(), NULL, 10);
          if (server.arg(i).length() && !errno)
          {
            mclockp->brightness = pval;
            if (mclockp->brightness > 100)
              mclockp->brightness = 100;
            prevEpoch = 0;
          }
          else
            sprintf(outstr + strlen(outstr), "BRIGHT=%d\r\n", mclockp->brightness);
        }
        else if (server.argName(i) == "NMBRIGHT")
        {
          pval = strtol(server.arg(i).c_str(), NULL, 10);
          if (server.arg(i).length() && !errno)
          {
            mclockp->nnm_brightness = pval;
            if (mclockp->nm_brightness > 100)
              mclockp->nm_brightness = 100;
            prevEpoch = 0;
          }
          else
            sprintf(outstr + strlen(outstr), "NMBRIGHT=%d\r\n", mclockp->nm_brightness);
        }
        else if (server.argName(i) == "RED")
        {
          pval = strtol(server.arg(i).c_str(), NULL, 10);
          if (server.arg(i).length() && !errno)
          {
            mclockp->r = pval;
            if (mclockp->r > 100)
              mclockp->r = 100;
          }
          else
            sprintf(outstr + strlen(outstr), "RED=%d\r\n", mclockp->r);
        }
        else if (server.argName(i) == "GREEN")
        {
          pval = strtol(server.arg(i).c_str(), NULL, 10);
          if (server.arg(i).length() && !errno)
          {
            mclockp->g = pval;
            if (mclockp->g > 100)
              mclockp->g = 100;
          }
          else
            sprintf(outstr + strlen(outstr), "GREEN=%d\r\n", mclockp->g);
        }
        else if (server.argName(i) == "BLUE")
        {
          pval = strtol(server.arg(i).c_str(), NULL, 10);
          if (server.arg(i).length() && !errno)
          {
            mclockp->b = pval;
            if (mclockp->b > 100)
              mclockp->b = 100;
          }
          else
            sprintf(outstr + strlen(outstr), "BLUE=%d\r\n", mclockp->b);
        }
#else
        else if (server.argName(i) == "RED")
        {
          if (server.arg(i) == "on")
            mclockp->r = 100;
          else if (server.arg(i) == "off")
            mclockp->r = 0;
          else
            sprintf(outstr + strlen(outstr), "RED=%s\r\n", (mclockp->r) ? "on" : "off");
        }
        else if (server.argName(i) == "GREEN")
        {
          if (server.arg(i) == "on")
            mclockp->g = 100;
          else if (server.arg(i) == "off")
            mclockp->g = 0;
          else
            sprintf(outstr + strlen(outstr), "GREEN=%s\r\n", (mclockp->g) ? "on" : "off");
        }
        else if (server.argName(i) == "BLUE")
        {
          if (server.arg(i) == "on")
            mclockp->b = 100;
          else if (server.arg(i) == "off")
            mclockp->b = 0;
          else
            sprintf(outstr + strlen(outstr), "BLUE=%s\r\n", (mclockp->b) ? "on" : "off");
        }
#endif
        else if (server.argName(i) == "NMSTART")
        {
          pval = strtol(server.arg(i).c_str(), NULL, 10);
          if (server.arg(i).length() && !errno)
          {
            mclockp->nm_start = abs(pval);
            if (mclockp->nm_start > 23)
              mclockp->nm_start = 0;
          }
          else
            sprintf(outstr + strlen(outstr), "NMSTART=%d\r\n", mclockp->nm_start);
        }
        else if (server.argName(i) == "NMEND")
        {
          pval = strtol(server.arg(i).c_str(), NULL, 10);
          if (server.arg(i).length() && !errno)
          {
            mclockp->nm_end = abs(pval);
            if (mclockp->nm_end > 23)
              mclockp->nm_end = 0;
          }
          else
            sprintf(outstr + strlen(outstr), "NMEND=%d\r\n", mclockp->nm_end);
        }
      }
      *tstr2 = 0;
    }
    page_out();
    update_color();
  });
  // start web-server
  server.begin();
  Serial.println("Web server started!");
}

void loop()
{
  int tbright = brightness;
  unsigned long epoch = wtaClient.GetCurrentTime(false);
  //Serial.print("GetCurrentTime returned epoch = ");
  //Serial.println(epoch);
  if (epoch != 0)
    wtaClient.PrintTime();

  server.handleClient();            // handle HTTP-requests

  if (epoch != prevEpoch)
  {
    int hh = wtaClient.GetHours();
    int mm = wtaClient.GetMinutes();
    int ss = wtaClient.GetSeconds();

    if (mclockp->nm_start || mclockp->nm_end)
    {
      if (mclockp->nm_start > mclockp->nm_end)
      {
        tbright = ((hh >= mclockp->nm_start) || (hh < mclockp->nm_end)) ? mclockp->nm_brightness : mclockp->brightness;
      }
      else
      {
        tbright = ((hh >= mclockp->nm_start) && (hh < mclockp->nm_end)) ? mclockp->nm_brightness : mclockp->brightness;
      }
    }
    else
      tbright = mclockp->brightness;

    if (tbright != brightness)
    {
      brightness = tbright;
      update_color();
    }

    if (prevEpoch == 0)
    { // If we didn't have a previous time. Just draw it without morphing.
      Digits[DIG_S0]->Draw(ss % 10);
      Digits[DIG_S1]->Draw(ss / 10);
      Digits[DIG_M0]->Draw(mm % 10);
      Digits[DIG_M1]->Draw(mm / 10);
      Digits[DIG_H0]->Draw(hh % 10);
      Digits[DIG_H1]->Draw(hh / 10);
    }
    else
    {
      // epoch changes every miliseconds, we only want to draw when digits actually change.
      if (ss != prevss)
      {
        int s0 = ss % 10;
        int s1 = ss / 10;
        Digits[DIG_S0]->SetValue(s0);
        Digits[DIG_S1]->SetValue(s1);
        prevss = ss;
      }

      if (mm != prevmm)
      {
        int m0 = mm % 10;
        int m1 = mm / 10;
        Digits[DIG_M0]->SetValue(m0);
        Digits[DIG_M1]->SetValue(m1);
        prevmm = mm;
      }

      if (hh != prevhh)
      {
        int h0 = hh % 10;
        int h1 = hh / 10;
        Digits[DIG_H0]->SetValue(h0);
        Digits[DIG_H1]->SetValue(h1);
        prevhh = hh;
      }
    }
    prevEpoch = epoch;
  }

  for (int i = 0; i < NUM_DIGITS; i++)
    Digits[i]->Morph();
  delay(mclockp->fade);
}
