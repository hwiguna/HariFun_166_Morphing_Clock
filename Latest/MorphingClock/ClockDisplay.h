#ifndef CLOCKDISPLAY_H
#define CLOCKDISPLAY_H

#include <PxMatrix.h> 

#include "Digit.h"

class ClockDisplay{
  public:
    ClockDisplay();
    void display_network_info(const char access_point_name[], const char access_point_password[], const char access_point_ip[]);
    void display_config_info(const char timezone[], const char time_format[]);
    void clear_display();
    void show_text(const char text[]);
    void show_time(int hh, int mm, int ss, bool is_pm, bool military);
    void morph_time(int hh, int mm, int ss, bool is_pm, bool military);
    void show_weather(float current_temp, float min_temp, float max_temp, float feels_like_temp, char *conditions);

  private:
    uint16_t time_colour;
    Digit digit_0;
    Digit digit_1;
    Digit digit_2;
    Digit digit_3;
    Digit digit_4;
    Digit digit_5;  

    void show_ampm(bool is_pm); 
    void show_temp(float temp_display, uint16_t x_offset, uint16_t y_offset);
    void draw_weather_conditions(char *conditions);
};
#endif