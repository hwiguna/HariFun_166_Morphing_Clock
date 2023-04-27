#include <Ticker.h>
#include <PxMatrix.h>

#include "ClockDisplay.h"
#include "TinyFont.h"
#include "TinyIcons.h"
#include "Digit.h"

#define DEBUG

static const byte row_0 = 2+0*10;
static const byte row_1 = 2+1*10;
static const byte row_2 = 2+2*10;

Ticker display_ticker;

// Pins for LED MATRIX
PxMATRIX display(64, 32, 16, 2, 5, 4, 15, 12, 0);

static const uint16_t white = display.color565(25, 25, 25);
static const uint16_t red = display.color565(40, 0, 0);
static const uint16_t pink = display.color565(98, 10, 17);//(196, 2, 34);//(288, 0, 123);
static const uint16_t orange = display.color565(186, 22, 12);
static const uint16_t green = display.color565(0, 50, 0);
static const uint16_t blue = display.color565(0, 13, 25);
static const uint16_t light_blue = display.color565(16, 52, 155);
static const uint16_t grey = display.color565(20, 20, 20);  
static const uint16_t dark_grey = display.color565(40, 40, 40);
static const uint16_t cyan = display.color565(0, 25, 25);
static const uint16_t black = display.color565(0, 0, 0);
static const uint16_t yellow = display.color565(25, 25, 0);  
static const uint16_t purple = display.color565(25, 0, 25);   

bool use_ani = true;

ClockDisplay::ClockDisplay(){
  
  time_colour = cyan;

  display.begin(16);

  digit_0 = Digit(&display, 0, 52, 14, true, time_colour);
  digit_1 = Digit(&display, 0, 46, 14, true, time_colour);
  digit_2 = Digit(&display, 0, 37, 8, false, time_colour);
  digit_3 = Digit(&display, 0, 28, 8, false, time_colour);
  digit_4 = Digit(&display, 0, 16, 8, false, time_colour);
  digit_5 = Digit(&display, 0, 7, 8, false, time_colour);

  display_ticker.attach_ms(2, [](PxMATRIX *display_to_use) {
    display_to_use->display(70);
  }, &display);
  
  display.setTextColor(time_colour);
}

void ClockDisplay::display_network_info(const char access_point_name[], const char access_point_password[], const char access_point_ip[]){
  clear_display();
  display.setCursor(1, row_0); 
  display.print(F("AP"));
  display.setCursor(1+10, row_0); 
  display.print(F(":"));
  display.setCursor(1+10+5, row_0); 
  display.print(access_point_name);

  display.setCursor(1, row_1); 
  display.print(F("Pw"));
  display.setCursor(1+10, row_1); 
  display.print(F(":"));
  display.setCursor(1+10+5, row_1); 
  display.print(access_point_password);

  display.setCursor(1, row_2); 
  display.print(F("IP"));
  display.setCursor(1+10, row_2); 
  display.print(F(":"));
  TFDrawText (&display, access_point_ip, 1+10+5, row_2 + 1, time_colour);
}

void ClockDisplay::display_config_info(const char timezone[], const char time_format[]){
  clear_display();
  display.setCursor(1, row_0); 
  display.print(F("Connected!"));

  display.setCursor(1, row_1); 
  display.print(F("Zone:"));
  display.print(timezone);

  display.setCursor(1, row_2); 
  display.print(F("Military:"));
  display.print(time_format);
}

void ClockDisplay::clear_display(){
  display.fillScreen(display.color565(0, 0, 0));
}

void ClockDisplay::clear_time(){
  digit_0.clear();
  digit_1.clear();
  digit_2.clear();
  digit_3.clear();
  digit_4.clear();
  digit_5.clear();
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
    TFDrawText (&display, F("PM"), 49, 19, time_colour);
  }
  else{
    TFDrawText (&display, F("AM"), 49, 19, time_colour);
  }
}

void ClockDisplay::show_weather(float current_temp, float min_temp, float max_temp, float feels_like_temp, const char *conditions){

  Serial.print(F("showing the weather "));
  Serial.print(current_temp);
  Serial.print(F(" "));
  Serial.print(feels_like_temp);
  Serial.print(F(" "));
  Serial.print(min_temp);
  Serial.print(F(" "));
  Serial.print(max_temp);
  Serial.print(F(" "));
  Serial.println(conditions);
 
  if (current_temp == -10000)
  {
    Serial.println (F("draw_weather: No weather data available"));
  }
  else
  {
    show_temp(current_temp, 0, 1); //top left

    uint8_t x_offset = get_right_offset(feels_like_temp);

    show_temp(feels_like_temp, x_offset, 1); // top right

    show_temp(min_temp, 0, 26); // bottom left

    x_offset = get_right_offset(max_temp);
    show_temp(max_temp, x_offset, 26); // bottom right

    //weather conditions
    draw_weather_conditions(conditions);
  }
}

uint8_t ClockDisplay::get_right_offset(float temp){
  uint8_t x_offset = 55;

  if (temp >= 10){
      x_offset = 51;
  }

  return x_offset;
}

void ClockDisplay::show_temp(float temp, uint16_t x_offset, uint16_t y_offset){
  int rounded_temp = round(temp);

  char temp_display[5]; 
  sprintf(temp_display, "%iC", rounded_temp);
  
  uint16_t temp_colour = red;
    
  if (temp >= 18 && temp < 26)
    temp_colour = orange;
  else if (temp >= 6 && temp < 18)
    temp_colour = light_blue;
  else if (temp < 6)
    temp_colour = white;
  
  TFDrawText(&display, F("    "), x_offset, y_offset, 0);
  TFDrawText(&display, temp_display, x_offset, y_offset, temp_colour);
}

/*
 * icons set
 */
//icons 10x5: 10 cols, 5 rows
int moony_ico[50] = {
  //3 nuances: 0x18c3 < 0x3186 < 0x4a49
  0x0000, grey,   0x0000, 0x0000, 0x0000, white, 0x0000,    0x0000,    0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   0x0000,    0x0000, dark_grey,
  0x0000, 0x0000, 0x0000, grey,   white,  white,  dark_grey,0x0000,    0x0000, 0x0000,
  0x0000, 0x0000, grey,   grey,   white,  white,  white,    dark_grey, 0x0000, 0x0000,
  0x0000, 0x0000, grey,   white,  white,  white,  white,    dark_grey, 0x0000, 0x0000,
};

#ifdef USE_WEATHER_ANI
int moony1_ico[50] = {
  //3 nuances: 0x18c3 < 0x3186 < 0x4a49
  0x0000, 0x18c3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x4a49,
  0x0000, 0x0000, 0x0000, 0x4a49, 0x3186, 0x3186, 0x18c3, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x4a49, 0x4a49, 0x3186, 0x3186, 0x3186, 0x18c3, 0x0000, 0x0000,
  0x0000, 0x0000, 0x4a49, 0x3186, 0x3186, 0x3186, 0x3186, 0x18c3, 0x0000, 0x0000,
};

int moony2_ico[50] = {
  //3 nuances: 0x18c3 < 0x3186 < 0x4a49
  0x0000, 0x3186, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3186,
  0x0000, 0x0000, 0x0000, 0x4a49, 0x3186, 0x3186, 0x18c3, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x4a49, 0x4a49, 0x3186, 0x3186, 0x3186, 0x18c3, 0x0000, 0x0000,
  0x0000, 0x0000, 0x4a49, 0x3186, 0x3186, 0x3186, 0x3186, 0x18c3, 0x0000, 0x0000,
};
#endif

int sunny_ico[50] = {
  0x0000, 0x0000, 0x0000, yellow, 0x0000, 0x0000, yellow, 0x0000, 0x0000, 0x0000,
  0x0000, yellow, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, yellow, 0x0000,
  0x0000, 0x0000, 0x0000, yellow, yellow, yellow, yellow, 0x0000, 0x0000, 0x0000,
  yellow, 0x0000, yellow, yellow, yellow, yellow, yellow, yellow, 0x0000, yellow,
  0x0000, 0x0000, yellow, yellow, yellow, yellow, yellow, yellow, 0x0000, 0x0000,
};

#ifdef USE_WEATHER_ANI
int sunny1_ico[50] = {
  0x0000, 0x0000, 0x0000, 0xffe0, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000,
  0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0x0000,
  0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000,
  0xffe0, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0xffff,
  0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000,
};

int sunny2_ico[50] = {
  0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0xffe0, 0x0000, 0x0000, 0x0000,
  0x0000, 0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000,
  0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000,
  0xffff, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0xffe0,
  0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000,
};
#endif

int cloudy_ico[50] = {
  0x0000, 0x0000, 0x0000, yellow, 0x0000, 0x0000, yellow, 0x0000, 0x0000, 0x0000,
  0x0000, yellow, 0x0000, 0x0000, 0x0000, 0x0000, white,  white,  yellow, 0x0000,
  0x0000, 0x0000, 0x0000, yellow, yellow, white,  white,  white,  white,  0x0000,
  yellow, 0x0000, yellow, yellow, yellow, white,  white,  white,  white,  white,
  0x0000, 0x0000, yellow, yellow, yellow, yellow, white,  white,  white,  white,
};

#ifdef USE_WEATHER_ANI
int cloudy1_ico[50] = {
  0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0xffe0, 0x0000, 0x0000, 0x0000,
  0x0000, 0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0xc618, 0xc618, 0xffff, 0x0000,
  0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xc618, 0xc618, 0xc618, 0xc618, 0x0000,
  0xffff, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xc618, 0xc618, 0xc618, 0xc618, 0xc618,
  0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xc618, 0xc618, 0xc618, 0xc618,
};

int cloudy2_ico[50] = {
  0x0000, 0x0000, 0x0000, 0xffe0, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000,
  0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0xc618, 0xc618, 0xffe0, 0x0000,
  0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xc618, 0xc618, 0xc618, 0xc618, 0x0000,
  0xffe0, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xc618, 0xc618, 0xc618, 0xc618, 0xc618,
  0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xc618, 0xc618, 0xc618, 0xc618,
};
#endif

int ovrcst_ico[50] = {
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, grey,  grey,  0x0000, 0x0000,
  0x0000, 0x0000, grey,   grey,   0x0000, grey,   grey,  grey,  grey,   0x0000,
  0x0000, grey,   white,  white,  grey,   white,  white, white, grey,   0x0000,
  0x0000, grey,   white,  white,  white,  white,  white, white, grey,   0x0000,
  0x0000, 0x0000, grey,   grey,   grey,   grey,   grey,  grey,  0x0000, 0x0000,
};

#ifdef USE_WEATHER_ANI
int ovrcst1_ico[50] = {
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xc618, 0xc618, 0x0000, 0x0000,
  0x0000, 0x0000, 0xc618, 0xc618, 0x0000, 0xc618, 0xc618, 0xc618, 0xc618, 0x0000,
  0x0000, 0xc618, 0xc618, 0xc618, 0xc618, 0xffff, 0xffff, 0xffff, 0xc618, 0x0000,
  0x0000, 0xc618, 0xc618, 0xc618, 0xffff, 0xffff, 0xffff, 0xffff, 0xc618, 0x0000,
  0x0000, 0x0000, 0xc618, 0xc618, 0xc618, 0xc618, 0xc618, 0xc618, 0x0000, 0x0000,
};

int ovrcst2_ico[50] = {
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xc618, 0xc618, 0x0000, 0x0000,
  0x0000, 0x0000, 0xc618, 0xc618, 0x0000, 0xc618, 0xc618, 0xc618, 0xc618, 0x0000,
  0x0000, 0xc618, 0xffff, 0xffff, 0xc618, 0xc618, 0xc618, 0xc618, 0xc618, 0x0000,
  0x0000, 0xc618, 0xffff, 0xffff, 0xffff, 0xc618, 0xc618, 0xc618, 0xc618, 0x0000,
  0x0000, 0x0000, 0xc618, 0xc618, 0xc618, 0xc618, 0xc618, 0xc618, 0x0000, 0x0000,
};
#endif

int thndr_ico[50] = {
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, grey,  grey,  0x0000, 0x0000,
  0x0000, 0x0000, grey,   grey,   0x0000, grey,   grey,  grey,  grey,   0x0000,
  0x0000, grey,   white,  white,  yellow,   yellow, white, white, grey,   0x0000,
  0x0000, grey,   white,  yellow, yellow, white,  white, white, grey,   0x0000,
  0x0000, 0x0000, 0x0000, yellow, 0x0000, 0x0000, 0x0000,0x0000, 0x0000, 0x0000,
};

int rain_ico[50] = {
  0x0000, 0x0000, grey, dark_grey, grey, 0x0000, dark_grey, 0x0000, 0x0000, 0x0000,
  0x0000, grey,   dark_grey, grey, grey, dark_grey, dark_grey, dark_grey, 0x0000, 0x0000,
  0x0000, 0x0000,   grey, dark_grey, grey, dark_grey, dark_grey, dark_grey, 0x0000, 0x0000,
  0x0000, light_blue, 0x0000, light_blue, 0x0000, light_blue, 0x0000, light_blue, 0x0000, 0x0000,
  0x0000, 0x0000, light_blue, 0x0000, light_blue, 0x0000, light_blue, 0x0000, 0x0000, 0x0000,
};

#ifdef USE_WEATHER_ANI
int rain1_ico[50] = {
  0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x041f, 0x0000,
  0x041f, 0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x041f,
  0x0000, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000,
  0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x041f,
};

int rain2_ico[50] = {
  0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x041f,
  0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x041f, 0x0000,
  0x041f, 0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x041f,
  0x0000, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000,
  0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x0000,
};

int rain3_ico[50] = {
  0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x041f,
  0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x041f, 0x0000,
  0x041f, 0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x041f,
  0x0000, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000,
};

int rain4_ico[50] = {
  0x0000, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000,
  0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x041f,
  0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x041f, 0x0000,
  0x041f, 0x0000, 0x041f, 0x0000, 0x0000, 0x0000, 0x041f, 0x0000, 0x0000, 0x041f,
};
#endif

int snow_ico[50] = {
  white,  0x0000, white,  0x0000, 0x0000, 0x0000, white,  0x0000, 0x0000, white,
  0x0000, 0x0000, 0x0000, 0x0000, white,  0x0000, 0x0000, white,  0x0000, 0x0000,
  0x0000, white,  0x0000, 0x0000, 0x0000, white,  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, white,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, white,
  0x0000, white,  0x0000, 0x0000, 0x0000, 0x0000, white,  0x0000, white,  0x0000,
};

#ifdef USE_WEATHER_ANI
int snow1_ico[50] = {
  0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0xc618, 0x0000,
  0xc618, 0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0xc618,
  0x0000, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000,
  0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xc618,
};

int snow2_ico[50] = {
  0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xc618,
  0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0xc618, 0x0000,
  0xc618, 0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0xc618,
  0x0000, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000,
  0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0x0000,
};

int snow3_ico[50] = {
  0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xc618,
  0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0xc618, 0x0000,
  0xc618, 0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0xc618,
  0x0000, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000,
};

int snow4_ico[50] = {
  0x0000, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000,
  0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xc618,
  0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0xc618, 0x0000,
  0xc618, 0x0000, 0xc618, 0x0000, 0x0000, 0x0000, 0xc618, 0x0000, 0x0000, 0xc618,
};
#endif

#ifdef USE_WEATHER_ANI
int *suny_ani[] = {sunny_ico, sunny1_ico, sunny2_ico, sunny1_ico, sunny2_ico};
int *clod_ani[] = {cloudy_ico, cloudy1_ico, cloudy2_ico, cloudy1_ico, cloudy2_ico};
int *ovct_ani[] = {ovrcst_ico, ovrcst1_ico, ovrcst2_ico, ovrcst1_ico, ovrcst2_ico};
int *rain_ani[] = {rain_ico, rain1_ico, rain2_ico, rain3_ico, rain4_ico};
int *thun_ani[] = {thndr_ico, rain1_ico, rain2_ico, rain3_ico, rain4_ico};
int *snow_ani[] = {snow_ico, snow1_ico, snow2_ico, snow3_ico, snow4_ico};
int *mony_ani[] = {moony_ico, moony1_ico, moony2_ico, moony1_ico, moony2_ico};
#else
int *suny_ani[] = {sunny_ico, sunny_ico, sunny_ico, sunny_ico, sunny_ico};
int *clod_ani[] = {cloudy_ico, cloudy_ico, cloudy_ico, cloudy_ico, cloudy_ico};
int *ovct_ani[] = {ovrcst_ico, ovrcst_ico, ovrcst_ico, ovrcst_ico, ovrcst_ico};
int *rain_ani[] = {rain_ico, rain_ico, rain_ico, rain_ico, rain_ico};
int *thun_ani[] = {thndr_ico, rain_ico, rain_ico, rain_ico, rain_ico};
int *snow_ani[] = {snow_ico, snow_ico, snow_ico, snow_ico, snow_ico};
int *mony_ani[] = {moony_ico, moony_ico, moony_ico, moony_ico, moony_ico};
#endif

int xo = 1, yo = 26;
char daytime = 1;

void ClockDisplay::draw_weather_conditions(const char *conditions){
  Serial.print(F("Weather conditions "));
  Serial.println(conditions);
  
  //cleanup previous cond
  xo = 27; 
  yo = 1;
  
  TFDrawText(&display, F("     "), xo, yo, 0);
  
  //xo = 4*TF_COLS; yo = 1;

  int *conditions_icon = NULL;

  use_ani = true;

  if(!strncmp(conditions, "Clear", strlen(conditions))){
    if (!daytime)
        conditions_icon = moony_ico;
    else
      conditions_icon = sunny_ico;
  }
  else if(!strncmp(conditions, "Clouds", strlen(conditions))){
    conditions_icon = cloudy_ico;
  }
  else if(!strncmp(conditions, "Overcast", strlen(conditions))){
    conditions_icon = ovrcst_ico;
  }
  else if(!strncmp(conditions, "Rain", strlen(conditions)) || !strncmp(conditions, "Drizzle", strlen(conditions))){
    conditions_icon = rain_ico;
  }
  else if(!strncmp(conditions, "Thunderstorm", strlen(conditions))){
    conditions_icon = thndr_ico;
  }
  else if(!strncmp(conditions, "Snow", strlen(conditions))){
    conditions_icon = snow_ico;
  }
  else{

    use_ani = false;

    Serial.println(F("draw_weather_conditions: !weather condition icon unknown, show text "));
    
    TFDrawText (&display,"     ", xo, yo, 0);
    TFDrawText (&display, conditions, 27, yo, dark_grey);
  }

  if (conditions_icon != NULL){
    draw_icon(&display, conditions_icon, 27, yo, 10, 5);
  }
}

void draw_animations(char *conditions, int stp){
//#ifdef USE_ICONS
  //weather icon animation
  int xo = 27; 
  int yo = 1;
  
  if (use_ani)
  {
    int *weather_icon = NULL;
    
    if(!strncmp(conditions, "Clear", strlen(conditions))){
      if (!daytime)
        weather_icon = mony_ani[stp%5];
      else
        weather_icon = suny_ani[stp%5];
    }
    else if(!strncmp(conditions, "Clouds", strlen(conditions))){
      weather_icon = clod_ani[stp%5];
    }
    else if(!strncmp(conditions, "Overcast", strlen(conditions))){
      weather_icon = ovct_ani[stp%5];
    }
    else if(!strncmp(conditions, "Rain", strlen(conditions)) || 
            !strncmp(conditions, "Drizzle", strlen(conditions))){
      weather_icon = rain_ani[stp%5];
    }
    else if(!strncmp(conditions, "Thunderstorm", strlen(conditions))){
      weather_icon = thun_ani[stp%5];
    }
    else if(!strncmp(conditions, "Snow", strlen(conditions))){
      weather_icon = snow_ani[stp%5];
    }
  
    //draw animation
    if (weather_icon)
      draw_icon(&display, weather_icon, xo, yo, 10, 5);
  }
//#endif
}

