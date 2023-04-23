#include <Ticker.h>
#include <PxMatrix.h>

#include "ClockDisplay.h"
#include "TinyFont.h"
#include "Digit.h"

#define DEBUG

static const byte row_0 = 2+0*10;
static const byte row_1 = 2+1*10;
static const byte row_2 = 2+2*10;

Ticker display_ticker;

// Pins for LED MATRIX
PxMATRIX display(64, 32, 16, 2, 5, 4, 15, 12, 0);

ClockDisplay::ClockDisplay(){
  
  time_colour = display.color565(0, 255, 255);

  display.begin(16);

  digit_0 = Digit(&display, 0, 63 - 16*1, 14, true, time_colour);
  digit_1 = Digit(&display, 0, 63 - 11*2, 14, true, time_colour);
  digit_2 = Digit(&display, 0, 63 - 4 - 9*3, 8, false, time_colour);
  digit_3 = Digit(&display, 0, 63 - 4 - 9*4, 8, false, time_colour);
  digit_4 = Digit(&display, 0, 63 - 7 - 9*5, 8, false, time_colour);
  digit_5 = Digit(&display, 0, 63 - 7 - 9*6, 8, false, time_colour);

  display_ticker.attach_ms(2, [](PxMATRIX *display_to_use) {
    display_to_use->display(70);
  }, &display);
  
  //clear_display();
  display.setTextColor(time_colour);
}

void ClockDisplay::display_network_info(const char access_point_name[], const char access_point_password[], const char access_point_ip[]){
  clear_display();
  display.setCursor(1, row_0); 
  display.print("AP");
  display.setCursor(1+10, row_0); 
  display.print(":");
  display.setCursor(1+10+5, row_0); 
  display.print(access_point_name);

  display.setCursor(1, row_1); 
  display.print("Pw");
  display.setCursor(1+10, row_1); 
  display.print(":");
  display.setCursor(1+10+5, row_1); 
  display.print(access_point_password);

  display.setCursor(1, row_2); 
  display.print("IP");
  display.setCursor(1+10, row_2); 
  display.print(":");
  TFDrawText (&display, access_point_ip, 1+10+5, row_2 + 1, time_colour);
}

void ClockDisplay::display_config_info(const char timezone[], const char time_format[]){
  clear_display();
  display.setCursor(1, row_0); 
  display.print("Connected!");

  display.setCursor(1, row_1); 
  display.print("Zone:");
  display.print(timezone);

  display.setCursor(1, row_2); 
  display.print("Military:");
  display.print(time_format);
}

void ClockDisplay::clear_display(){
  display.fillScreen(display.color565(0, 0, 0));
}

void ClockDisplay::show_text(const char *text){
  clear_display();
  display.setCursor(2, row_0);
  display.print(text);
}

void ClockDisplay::show_time(int hh, int mm, int ss, bool is_pm, bool military){
  clear_display();
  digit_0.draw(ss % 10);
  digit_1.draw(ss / 10);
  digit_2.draw(mm % 10);
  digit_3.draw(mm / 10);
  digit_3.draw_colon(time_colour);
  digit_4.draw(hh % 10);
  digit_5.draw(hh / 10);

  if (!military){
    
    show_ampm(is_pm);
  }
}

void ClockDisplay::morph_time(int hh, int mm, int ss, bool is_pm, bool military){
  
  int s0 = ss % 10;
  int s1 = ss / 10;
  if (s0!=digit_0.value()) digit_0.morph(s0);
  if (s1!=digit_1.value()) digit_1.morph(s1);
        
  int m0 = mm % 10;
  int m1 = mm / 10;
  if (m0!=digit_2.value()) digit_2.morph(m0);
  if (m1!=digit_3.value()) digit_3.morph(m1);
      
  int h0 = hh % 10;
  int h1 = hh / 10;
  if (h0!=digit_4.value()) digit_4.morph(h0);
  if (h1!=digit_5.value()) digit_5.morph(h1);

  if (military){
    show_ampm(is_pm);
  }
}

void ClockDisplay::show_ampm(bool is_pm){
  if (is_pm){
    TFDrawText (&display, F("PM"), 44, 19, time_colour);
  }
  else{
    TFDrawText (&display, F("AM"), 44, 19, time_colour);
  }
}

void ClockDisplay::show_weather(float current_temp, float min_temp, float max_temp, float feels_like_temp){
  uint16_t white = display.color565 (255, 255, 255);
  uint16_t red = display.color565 (255, 0, 0);
  uint16_t green = display.color565 (0, 255, 0);
  uint16_t blue = display.color565 (0, 0, 255);
  uint16_t dark_grey = display.color565 (30, 30, 30);
  
  uint8_t x_offset = 0;
  uint8_t y_offset = 1;

#ifdef DEBUG
  Serial.print("showing the weather ");
  Serial.print(current_temp);
  Serial.print(" ");
  Serial.print(feels_like_temp);
  Serial.print(" ");
  Serial.print(min_temp);
  Serial.print(" ");
  Serial.println(max_temp);
#endif
 
  if (current_temp == -10000)
  {
    Serial.println (F("draw_weather: No weather data available"));
  }
  else
  {
    show_temp(current_temp, 0, 1); //top left
    show_temp(feels_like_temp, 55, 1); // top right
    show_temp(min_temp, 0, 26); // bottom left
    show_temp(max_temp, 55, 26); // bottom right

    //weather conditions
    /*draw_weather_conditions ();*/
  }

}

void ClockDisplay::show_temp(float temp, uint16_t x_offset, uint16_t y_offset){
  uint16_t white = display.color565 (100, 100, 100);
  uint16_t red = display.color565 (255, 0, 0);
  uint16_t green = display.color565 (0, 255, 0);
  uint16_t blue = display.color565 (0, 0, 255);
  uint16_t dark_grey = display.color565 (30, 30, 30);

  int rounded_temp = round(temp);

  char temp_display[5]; 
  sprintf(temp_display, "%iC", rounded_temp);
  
  uint16_t temp_colour = red;
    
  if (temp >= 18 && temp < 26)
    temp_colour = green;
  else if (temp >= 6 && temp < 18)
    temp_colour = blue;
  else if (temp < 6)
    temp_colour = white;
  
  TFDrawText(&display, "    ", x_offset, y_offset, 0);
  TFDrawText(&display, temp_display, x_offset, y_offset, temp_colour);
}
