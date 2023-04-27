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

#include <ArduinoJson.h>

#include "NTPClient.h"
#include "Digit.h"
#include "ClockDisplay.h"

ClockDisplay clock_display;

NTPClient ntp_client;
unsigned long prev_epoch;

static const uint8_t weather_conditions_len = 50;

int weather_refresh_interval_mins = 1; // TODO: move to config
/*float weather_current_temp = -1000;
float weather_max_temp = -1000;
float weather_min_temp = -1000;
float weather_feels_like_temp = -1000;
float weather_wind_speed = -1000; 
char weather_conditions[weather_conditions_len];*/



void setup() {
  Serial.begin(9600);

  clock_display = ClockDisplay();

  ntp_client.Setup(&clock_display);

  //get_weather();
  //clock_display.show_weather(weather_current_temp, weather_min_temp, weather_max_temp, weather_feels_like_temp, weather_conditions);
}

void loop() {
  unsigned long epoch = ntp_client.GetCurrentTime();
  
  /*float weather_current_temp = -1000;
  float weather_max_temp = -1000;
  float weather_min_temp = -1000;
  float weather_feels_like_temp = -1000;
  float weather_wind_speed = -1000; 
  char weather_conditions[weather_conditions_len];*/

  /*Serial.print("Epoch ");
  Serial.print(epoch);
  Serial.print(" Previous Epoch ");
  Serial.println(prev_epoch);*/
  if (epoch != 0) ntp_client.PrintTime();

  if (epoch != prev_epoch){
    int current_hour = ntp_client.GetHours();
    int current_mins = ntp_client.GetMinutes();
    int current_seconds = ntp_client.GetSeconds();
    bool is_pm = ntp_client.GetIsPM();
    bool is_military = ntp_client.GetIsMilitary();

    if (prev_epoch == 0){ // If we didn't have a previous time. Just draw it without morphing.
      clock_display.clear_display();
      clock_display.show_time(current_hour, current_mins, current_seconds, is_pm, is_military);//Need to not hard code the 'false' here
    }
    else{
      // epoch changes every miliseconds, we only want to draw when digits actually change.
      clock_display.morph_time(current_hour, current_mins, current_seconds, is_pm, is_military);
    }

    if(prev_epoch == 0 || (current_seconds == 30 && (current_mins % weather_refresh_interval_mins == 0))){
      show_weather();
      //clock_display.show_weather(weather_current_temp, weather_min_temp, weather_max_temp, weather_feels_like_temp, weather_conditions);
    }

    //weather animations
    /*if ((cm - last) > 150)
    {
      //Serial.println(millis() - last);
      last = cm;
      i++;
      
      draw_animations (i);
      
    }*/

    prev_epoch = epoch;
  }
}

char weather_api_key[] = "ca1a7bdba9574adefbba2361ba980c6e";
char weather_location[] = "Toronto,CA"; 
char weather_server[] = "api.openweathermap.org";

WiFiClient client;


String condS = "";

void show_weather()
{
  Serial.print("getWeather: Connecting to weather server ");
  Serial.println(weather_server);
  
  if (client.connect (weather_server, 80))
  { 
    Serial.print("getWeather: WiFi connected to ");
    Serial.println(weather_server); 
    // tryin getting rid of + by using a second print/println
    client.print("GET /data/2.5/weather?"); 
    client.print("q=");
    client.print(weather_location); 
    client.print("&appid=");
    client.print(weather_api_key); 
    client.println("&cnt=1&units=metric"); 
    client.print("Host: ");
    client.println(weather_server); 
    client.println("Connection: close");
    client.println(); 
  } 
  else 
  { 
    Serial.println ("getWeather: Unable to connect.");
    return;
  } 
  
  delay (1000);
  
  String weather_info = client.readStringUntil('\n');
  if (!weather_info.length ())
    Serial.println (F("getWeather: unable to retrieve weather data"));
  else
  {
    Serial.println(F("getWeather: Weather response:")); 
    Serial.println(weather_info); 

    DynamicJsonDocument weather_json(1024);
    auto error = deserializeJson(weather_json, weather_info);

    if (error) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
    }

//TODO: may want to make sure that the values are there - could crash

    float weather_current_temp = weather_json[F("main")][F("temp")];
    float weather_max_temp = weather_json[F("main")][F("temp_max")];
    float weather_min_temp = weather_json[F("main")][F("temp_min")];
    float weather_feels_like_temp = weather_json[F("main")][F("feels_like")];
    float weather_wind_speed = weather_json[F("wind")][F("speed")];
    const char *weather_conditions = weather_json[F("weather")][0][F("main")];

    //const char *weather_conditions_temp = weather_json[F("weather")][0][F("main")];
    //strncpy(weather_conditions, weather_conditions_temp, weather_conditions_len);
    
    Serial.print(F("Weather: "));
    Serial.println(weather_conditions);
    Serial.print(F("Temp: "));
    Serial.println(weather_current_temp);
    Serial.print(F("Temp Max: "));
    Serial.println(weather_max_temp);
    Serial.print(F("Temp Min: "));
    Serial.println(weather_min_temp);
    Serial.print(F("Feels Like: "));
    Serial.println(weather_feels_like_temp);
    Serial.print(F("Wind Speed: "));
    Serial.println(weather_wind_speed);

    clock_display.show_weather(weather_current_temp, weather_min_temp, weather_max_temp, weather_feels_like_temp, weather_conditions);
  }

}




