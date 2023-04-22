#include <Ticker.h>
#include <PxMatrix.h>

#include "ClockDisplay.h"
#include "TinyFont.h"
#include "Digit.h"

static const byte row0 = 2+0*10;
static const byte row1 = 2+1*10;
static const byte row2 = 2+2*10;

Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

#define TIME_POS_X 63
#define SEC_HEIGHT 3
#define HOUR_HEIGHT 6
#define MIN_HEIGHT HOUR_HEIGHT

// Pins for LED MATRIX
PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

ClockDisplay::ClockDisplay(){
  
  timeColour = display.color565(0, 255, 255);

  display.begin(16);

  digit0 = Digit(&display, 0, TIME_POS_X - 16*1, 14, true, timeColour);
  digit1 = Digit(&display, 0, TIME_POS_X - 11*2, 14, true, timeColour);
  digit2 = Digit(&display, 0, TIME_POS_X - 4 - 9*3, 8, false, timeColour);
  digit3 = Digit(&display, 0, TIME_POS_X - 4 - 9*4, 8, false, timeColour);
  digit4 = Digit(&display, 0, TIME_POS_X - 7 - 9*5, 8, false, timeColour);
  digit5 = Digit(&display, 0, TIME_POS_X - 7 - 9*6, 8, false, timeColour);

  display_ticker.attach_ms(2, [](PxMATRIX *displayToUse) {
    displayToUse->display(70);
  }, &display);
  
  //clearDisplay();
  display.setTextColor(timeColour);
}

void ClockDisplay::displayNetworkInfo(const char accessPointName[], const char accessPointPassword[], const char accessPointIP[]){
  clearDisplay();
  display.setCursor(1, row0); 
  display.print("AP");
  display.setCursor(1+10, row0); 
  display.print(":");
  display.setCursor(1+10+5, row0); 
  display.print(accessPointName);

  display.setCursor(1, row1); 
  display.print("Pw");
  display.setCursor(1+10, row1); 
  display.print(":");
  display.setCursor(1+10+5, row1); 
  display.print(accessPointPassword);

  display.setCursor(1, row2); 
  display.print("IP");
  display.setCursor(1+10, row2); 
  display.print(":");
  TFDrawText (&display, accessPointIP, 1+10+5, row2 + 1, timeColour);
}

void ClockDisplay::displayConfigInfo(const char timezone[], const char timeFormat[]){
  clearDisplay();
  display.setCursor(1, row0); 
  display.print("Connected!");

  display.setCursor(1, row1); 
  display.print("Zone:");
  display.print(timezone);

  display.setCursor(1, row2); 
  display.print("Military:");
  display.print(timeFormat);
}

void ClockDisplay::clearDisplay(){
  display.fillScreen(display.color565(0, 0, 0));
}

void ClockDisplay::showText(const char *text){
  clearDisplay();
  display.setCursor(2, row0);
  display.print(text);
}

void ClockDisplay::showTime(int hh, int mm, int ss, bool isPM, bool military){
  clearDisplay();
  digit0.Draw(ss % 10);
  digit1.Draw(ss / 10);
  digit2.Draw(mm % 10);
  digit3.Draw(mm / 10);
  digit3.DrawColon(timeColour);
  digit4.Draw(hh % 10);
  digit5.Draw(hh / 10);

  if (!military){
    
    showAMPM(isPM);
  }
}

void ClockDisplay::morphTime(int hh, int mm, int ss, bool isPM, bool military){
  
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

  if (military){
    showAMPM(isPM);
  }
}

void ClockDisplay::showAMPM(bool isPM){
  if (isPM){
    TFDrawText (&display, "PM", 44, 19, timeColour);
  }
  else{
    TFDrawText (&display, "AM", 44, 19, timeColour);
  }
}

int xo = 1, yo = 26;

void ClockDisplay::show_weather(float current_temp, float min_temp){
  int cc_wht = display.color565 (255, 255, 255);
  int cc_red = display.color565 (255, 0, 0);
  int cc_grn = display.color565 (0, 255, 0);
  int cc_blu = display.color565 (0, 0, 255);
  int cc_dgr = display.color565 (30, 30, 30);
  
  Serial.println ("showing the weather");
  xo = 0; yo = 1;
  TFDrawText (&display, String("                "), xo, yo, cc_dgr);
  
  if (current_temp == -10000)
  {
    Serial.println ("draw_weather: No weather data available");
  }
  else
  {
    int lcc = cc_red;
    
    if (current_temp < 26)
      lcc = cc_grn;
    if (current_temp < 18)
      lcc = cc_blu;
    if (current_temp < 6)
      lcc = cc_wht;
    
    //
    int current_temp_round = round(current_temp);
    String lstr = String(current_temp_round) + String("C");
    Serial.print("temperature: ");
    Serial.println (lstr);
    TFDrawText(&display, lstr, xo, yo, lcc);
    
    xo = 0*TF_COLS; yo = 26;
    TFDrawText(&display, "   ", xo, yo, 0);
    int min_temp_round = round(min_temp);
    lstr = String(min_temp_round);// + String((*u_metric=='Y')?"C":"F");
    //blue if negative
    int ct = cc_dgr;
    if (min_temp < 0)
    {
      ct = cc_blu;
      lstr = String(-min_temp);// + String((*u_metric=='Y')?"C":"F");
    }
    Serial.print("temp min: ");
    Serial.println(lstr);
    TFDrawText(&display, lstr, xo, yo, ct);

    //weather conditions
    //-humidity
    /*lcc = cc_red;
    if (humiM < 65)
      lcc = cc_grn;
    if (humiM < 35)
      lcc = cc_blu;
    if (humiM < 15)
      lcc = cc_wht;
    lstr = String (humiM) + "%";
    xo = 8*TF_COLS;
    TFDrawText (&display, lstr, xo, yo, lcc);
    //-pressure
    lstr = String (presM);
    xo = 12*TF_COLS;
    TFDrawText (&display, lstr, xo, yo, cc_blu);
    //draw temp min/max
    if (tempMin > -10000)
    {
      xo = 0*TF_COLS; yo = 26;
      TFDrawText (&display, "   ", xo, yo, 0);
      lstr = String (tempMin);// + String((*u_metric=='Y')?"C":"F");
      //blue if negative
      int ct = cc_dgr;
      if (tempMin < 0)
      {
        ct = cc_blu;
        lstr = String (-tempMin);// + String((*u_metric=='Y')?"C":"F");
      }
      Serial.print ("temp min: ");
      Serial.println (lstr);
      TFDrawText (&display, lstr, xo, yo, ct);
    }
    if (tempMax > -10000)
    {
      TFDrawText (&display, "   ", 13*TF_COLS, yo, 0);
      //move the text to the right or left as needed
      xo = 14*TF_COLS; yo = 26;
      if (tempMax < 10)
        xo = 15*TF_COLS;
      if (tempMax > 99)
        xo = 13*TF_COLS;
      lstr = String (tempMax);// + String((*u_metric=='Y')?"C":"F");
      //blue if negative
      int ct = cc_dgr;
      if (tempMax < 0)
      {
        ct = cc_blu;
        lstr = String (-tempMax);// + String((*u_metric=='Y')?"C":"F");
      }
      Serial.print ("temp max: ");
      Serial.println (lstr);
      TFDrawText (&display, lstr, xo, yo, ct);
    }
    //weather conditions
    draw_weather_conditions ();*/
  }
}
