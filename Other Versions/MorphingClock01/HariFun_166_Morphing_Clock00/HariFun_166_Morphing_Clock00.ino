#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "NTPClient.h"

NTPClient ntpClient;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  ntpClient.Setup();
}

void loop()
{
  Serial.println("");  
  unsigned long epoch = ntpClient.GetCurrentEpoch();
  Serial.print("GetCurrentEpoch returned epoch = ");
  Serial.println(epoch);
  if (epoch != 0) ntpClient.PrintTime();

  delay(1000);
}


