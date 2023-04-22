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

ClockDisplay clockDisplay;

NTPClient ntpClient;
unsigned long prevEpoch;

int weather_refresh_interval_mins = 1; // TODO: move to config
float weather_current_temp = -1000;
float weatherMaxTemp = -1000;
float weather_min_temp = -1000;
float weatherFeelsLike = -1000;
float weatherWindSpeed = -1000; 
int weatherCondIndex = -1;  //-1 - undefined, 0 - unk, 1 - sunny, 2 - cloudy, 3 - overcast, 4 - rainy, 5 - thunders, 6 - snow

void setup() {
  Serial.begin(9600);

  clockDisplay = ClockDisplay();

  ntpClient.Setup(&clockDisplay);

  get_weather();
  clockDisplay.show_weather(weather_current_temp, weather_min_temp);
}

void loop() {
  unsigned long epoch = ntpClient.GetCurrentTime();
  
  if (epoch != 0) ntpClient.PrintTime();

  if (epoch != prevEpoch){
    int hh = ntpClient.GetHours();
    int mm = ntpClient.GetMinutes();
    int ss = ntpClient.GetSeconds();
    bool isPM = ntpClient.GetIsPM();
    bool military = ntpClient.GetIsMilitary();

    if (prevEpoch == 0){ // If we didn't have a previous time. Just draw it without morphing.
      clockDisplay.showTime(hh, mm, ss, isPM, military);//Need to not hard code the 'false' here
    }
    else{
      // epoch changes every miliseconds, we only want to draw when digits actually change.
      clockDisplay.morphTime(hh, mm, ss, isPM, military);
    }

    if(ss == 30 && (mm % weather_refresh_interval_mins == 0)){
      get_weather();
      clockDisplay.show_weather(weather_current_temp, weather_min_temp);
    }

    prevEpoch = epoch;
  }
}

char weatherAPIKey[] = "ca1a7bdba9574adefbba2361ba980c6e";
char weatherLocation[] = "Toronto,CA"; 
char weatherServer[] = "api.openweathermap.org";

WiFiClient client;


String condS = "";

void get_weather()
{
  Serial.print("getWeather: Connecting to weather server ");
  Serial.println(weatherServer);
  
  if (client.connect (weatherServer, 80))
  { 
    Serial.print("getWeather: WiFi connected to ");
    Serial.println(weatherServer); 
    // tryin getting rid of + by using a second print/println
    client.print("GET /data/2.5/weather?"); 
    client.print("q=");
    client.print(weatherLocation); 
    client.print("&appid=");
    client.print(weatherAPIKey); 
    client.println("&cnt=1&units=metric"); 
    client.print("Host: ");
    client.println(weatherServer); 
    client.println("Connection: close");
    client.println(); 
  } 
  else 
  { 
    Serial.println ("getWeather: Unable to connect.");
    return;
  } 
  
  delay (1000);
  
  String weatherInfo = client.readStringUntil('\n');
  if (!weatherInfo.length ())
    Serial.println (F("getWeather: unable to retrieve weather data"));
  else
  {
    Serial.println(F("getWeather: Weather response:")); 
    Serial.println(weatherInfo); 

    DynamicJsonDocument doc(1024);
    auto error = deserializeJson(doc, weatherInfo);

    if (error) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
    }

    const char *weatherCondition = doc[F("weather")][0][F("main")];
    weather_current_temp = doc[F("main")][F("temp")];
    weatherMaxTemp = doc[F("main")][F("temp_max")];
    weather_min_temp = doc[F("main")][F("temp_min")];
    weatherFeelsLike = doc[F("main")][F("feels_like")];
    weatherWindSpeed = doc[F("wind")][F("speed")];

    Serial.print(F("Weather: "));
    Serial.println(weatherCondition);
    Serial.print(F("Temp: "));
    Serial.println(weather_current_temp);
    Serial.print(F("Temp Max: "));
    Serial.println(weatherMaxTemp);
    Serial.print(F("Temp Min: "));
    Serial.println(weather_min_temp);
    Serial.print(F("Feels Like: "));
    Serial.println(weatherFeelsLike);
    Serial.print(F("Wind Speed: "));
    Serial.println(weatherWindSpeed);
  
    weatherCondIndex = get_condition_index(weatherCondition); 
  }
}

int get_condition_index(const char *weatherCondition){
  int conditionIndex = 0;
  
  if (strncmp(weatherCondition, "Clear", strlen(weatherCondition)))
    conditionIndex = 1;
  else if (strncmp(weatherCondition, "Clouds", strlen(weatherCondition)))
    conditionIndex = 2;
  else if (strncmp(weatherCondition, "Overcast", strlen(weatherCondition)))
    conditionIndex = 3;
  else if (strncmp(weatherCondition, "Rain", strlen(weatherCondition)) || 
           strncmp(weatherCondition, "Drizzle", strlen(weatherCondition)))
   conditionIndex = 4;
  else if (strncmp(weatherCondition, "Thunderstorm", strlen(weatherCondition)))
    conditionIndex = 5;
  else if (strncmp(weatherCondition, "Snow", strlen(weatherCondition)))
    conditionIndex = 6;

  return conditionIndex;
}


