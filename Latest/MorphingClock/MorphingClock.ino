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

int weather_refresh_interval_mins = 1; // TODO: move to config
float weather_current_temp = -1000;
float weather_max_temp = -1000;
float weather_min_temp = -1000;
float weather_feels_like = -1000;
float weather_wind_speed = -1000; 
int weather_cond__index = -1;  //-1 - undefined, 0 - unk, 1 - sunny, 2 - cloudy, 3 - overcast, 4 - rainy, 5 - thunders, 6 - snow

void setup() {
  Serial.begin(9600);

  clock_display = ClockDisplay();

  ntp_client.Setup(&clock_display);

  get_weather();
  clock_display.show_weather(weather_current_temp, weather_min_temp);
}

void loop() {
  unsigned long epoch = ntp_client.GetCurrentTime();
  
  if (epoch != 0) ntp_client.PrintTime();

  if (epoch != prev_epoch){
    int current_hour = ntp_client.GetHours();
    int current_mins = ntp_client.GetMinutes();
    int current_seconds = ntp_client.GetSeconds();
    bool is_pm = ntp_client.GetIsPM();
    bool is_military = ntp_client.GetIsMilitary();

    if (prev_epoch == 0){ // If we didn't have a previous time. Just draw it without morphing.
      clock_display.showTime(current_hour, current_mins, current_seconds, is_pm, is_military);//Need to not hard code the 'false' here
    }
    else{
      // epoch changes every miliseconds, we only want to draw when digits actually change.
      clock_display.morphTime(current_hour, current_mins, current_seconds, is_pm, is_military);
    }

    if(current_seconds == 30 && (current_mins % weather_refresh_interval_mins == 0)){
      get_weather();
      clock_display.show_weather(weather_current_temp, weather_min_temp);
    }

    prev_epoch = epoch;
  }
}

char weather_api_key[] = "ca1a7bdba9574adefbba2361ba980c6e";
char weather_location[] = "Toronto,CA"; 
char weather_server[] = "api.openweathermap.org";

WiFiClient client;


String condS = "";

void get_weather()
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

    const char *weather_condition = weather_json[F("weather")][0][F("main")];
    weather_current_temp = weather_json[F("main")][F("temp")];
    weather_max_temp = weather_json[F("main")][F("temp_max")];
    weather_min_temp = weather_json[F("main")][F("temp_min")];
    weather_feels_like = weather_json[F("main")][F("feels_like")];
    weather_wind_speed = weather_json[F("wind")][F("speed")];

    Serial.print(F("Weather: "));
    Serial.println(weather_condition);
    Serial.print(F("Temp: "));
    Serial.println(weather_current_temp);
    Serial.print(F("Temp Max: "));
    Serial.println(weather_max_temp);
    Serial.print(F("Temp Min: "));
    Serial.println(weather_min_temp);
    Serial.print(F("Feels Like: "));
    Serial.println(weather_feels_like);
    Serial.print(F("Wind Speed: "));
    Serial.println(weather_wind_speed);
  
    weather_cond__index = get_condition_index(weather_condition); 
  }
}

int get_condition_index(const char *weather_condition){
  int condition_index = 0;
  
  if (strncmp(weather_condition, "Clear", strlen(weather_condition)))
    condition_index = 1;
  else if (strncmp(weather_condition, "Clouds", strlen(weather_condition)))
    condition_index = 2;
  else if (strncmp(weather_condition, "Overcast", strlen(weather_condition)))
    condition_index = 3;
  else if (strncmp(weather_condition, "Rain", strlen(weather_condition)) || 
           strncmp(weather_condition, "Drizzle", strlen(weather_condition)))
   condition_index = 4;
  else if (strncmp(weather_condition, "Thunderstorm", strlen(weather_condition)))
    condition_index = 5;
  else if (strncmp(weather_condition, "Snow", strlen(weather_condition)))
    condition_index = 6;

  return condition_index;
}


