//=== WIFI MANAGER ===
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
char wifiManagerAPName[] = "MorphClk";
char wifiManagerAPPassword[] = "HariFun";


//== DOUBLE-RESET DETECTOR ==
#include <DoubleResetDetector.h>
#define DRD_TIMEOUT 10 // Second-reset must happen within 10 seconds of first reset to be considered a double-reset
#define DRD_ADDRESS 0 // RTC Memory Address for the DoubleResetDetector to use
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

//== SAVING CONFIG ==
#include "FS.h"
#include <ArduinoJson.h>
bool shouldSaveConfig = false; // flag for saving data

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


//=== NTP CLIENT ===
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "NTPClient.h"

#define DEBUG 0
const unsigned long askFrequency = 60*60*1000; // How frequent should we get current time? in miliseconds. 60minutes = 60*60s = 60*60*1000ms
unsigned long timeToAsk;
unsigned long timeToRead;
unsigned long lastEpoch; // We don't want to continually ask for epoch from time server, so this is the last epoch we received (could be up to an hour ago based on askFrequency)
unsigned long lastEpochTimeStamp; // What was millis() when asked server for Epoch we are currently using?
unsigned long nextEpochTimeStamp; // What was millis() when we asked server for the upcoming epoch
unsigned long currentTime;
char timezone[5] = "";
char military[3] = ""; // 24 hour mode? Y/N

const char* ntpServerName = "time.nist.gov";
IPAddress timeServerIP; // time.nist.gov NTP server address
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
WiFiUDP udp; // A UDP instance to let us send and receive packets over UDP
unsigned int localPort = 2390;      // local port to listen for UDP packets


void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  // You could indicate on your screen or by an LED you are in config mode here

  // We don't want the next time the boar resets to be considered a double reset
  // so we remove the flag
  drd.stop();
}


bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  strcpy(timezone, json["timezone"]);
  strcpy(military, json["military"]);
  return true;
}

bool saveConfig() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["timezone"] = timezone;
  json["military"] = military;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  Serial.print("timezone=");
  Serial.println(timezone);

  Serial.print("military=");
  Serial.println(military);

  json.printTo(configFile);
  return true;
}


NTPClient::NTPClient()
{
}


void NTPClient::Setup(PxMATRIX* d)
{
  //-- Config --
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount FS");
    return;
  }
  loadConfig();

  //-- Display --
  _display = d;
  _display->fillScreen(_display->color565(0, 0, 0));
  _display->setTextColor(_display->color565(0, 0, 255));
  //_display->setFont(&FreeMono9pt7b);
  //_display->setTextSize(1);
  const byte row0 = 2+0*10;
  const byte row1 = 2+1*10;
  const byte row2 = 2+2*10;

  //-- WiFiManager --
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  WiFiManagerParameter timeZoneParameter("timeZone", "Time Zone", timezone, 5); 
  wifiManager.addParameter(&timeZoneParameter);
  WiFiManagerParameter militaryParameter("military", "24Hr", military, 3); 
  wifiManager.addParameter(&militaryParameter);

  //-- Double-Reset --
  if (drd.detectDoubleReset()) {
    Serial.println("Double Reset Detected");
    digitalWrite(LED_BUILTIN, LOW);

    _display->setCursor(1, row0);     _display->print("AP");
    _display->setCursor(1+10, row0);    _display->print(":");
    _display->setCursor(1+10+5, row0);  _display->print(wifiManagerAPName);

    _display->setCursor(1, row1);     _display->print("Pw");
    _display->setCursor(1+10, row1);    _display->print(":");
    _display->setCursor(1+10+5, row1);  _display->print(wifiManagerAPPassword);

    _display->setCursor(1, row2); _display->print("192");
    _display->setCursor(1+3*6 -1, row2); _display->print(".168");
    _display->setCursor(1+3*6 -1 + 5+ 3*6, row2); _display->print(".4");
    _display->setCursor(1+3*6 -1 + 5+ 3*6 + 5 + 6, row2); _display->print(".1");

    wifiManager.startConfigPortal(wifiManagerAPName, wifiManagerAPPassword);

    _display->fillScreen(_display->color565(0, 0, 0));
  } 
  else 
  {
    Serial.println("No Double Reset Detected");
    digitalWrite(LED_BUILTIN, HIGH);

    _display->setCursor(2, row1);
    _display->print("Connecting");

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name wifiManagerAPName
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect(wifiManagerAPName, wifiManagerAPPassword);
  }
  
  //-- Status --
  _display->fillScreen(_display->color565(0, 0, 0));
  _display->setCursor(2, row0);
  _display->print("Connected!");
  Serial.println("WiFi connected");
  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());

  //-- Timezone --
  strcpy(timezone,timeZoneParameter.getValue());
  _display->setCursor(2, row1);
  _display->print("Zone:");
  _display->print(timezone);
  
  //-- Military --
  strcpy(military,militaryParameter.getValue());
  _display->setCursor(2, row2);
  _display->print("24Hr:");
  _display->print(military);

  if (shouldSaveConfig) {
    saveConfig();
  }
  drd.stop();
  
  delay(3000);
}


// send an NTP request to the time server at the given address
unsigned long NTPClient::sendNTPpacket(IPAddress& address)
{
  if (DEBUG) Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void NTPClient::AskCurrentEpoch()
{
  if (DEBUG) Serial.println("AskCurrentEpoch called");
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP);

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
}

unsigned long NTPClient::ReadCurrentEpoch()
{
  if (DEBUG) Serial.println("ReadCurrentEpoch called");
  int cb = udp.parsePacket();
  if (!cb) {
    if (DEBUG) Serial.println("no packet yet");
  }
  else {
    if (DEBUG) Serial.print("packet received, length=");
    if (DEBUG) Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    if (DEBUG) Serial.print("Seconds since Jan 1 1900 = " );
    if (DEBUG) Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    if (DEBUG) Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    lastEpoch = secsSince1900 - seventyYears; // 1530082740=Fake 6:59:00, 1530795595=Fake 12:59:55, 1530835195=fake 23:59:55
    lastEpochTimeStamp = nextEpochTimeStamp; // Now that we have a new epoch, finally update lastEpochTimeStamp so all time calculations would be offset by the time we ask for this new epoch.

    if (DEBUG) Serial.println(lastEpoch);
    return lastEpoch;
  }
}

unsigned long NTPClient::GetCurrentTime()
{
  //if (DEBUG) Serial.println("GetCurrentTime called");
  unsigned long timeNow = millis();
  if (timeNow > timeToAsk) { // Is it time to ask server for current time?
    if (DEBUG) Serial.println(" Time to ask");
    timeToAsk = timeNow + askFrequency; // Don't ask again for a while
    if (timeToRead == 0) { // If we have not asked...
      timeToRead = timeNow + 1000; // Wait one second for server to respond
      AskCurrentEpoch(); // Ask time server what is the current time?
      nextEpochTimeStamp  = millis(); // next epoch we receive is for "now".
    } 
  }

  if (timeToRead>0 && timeNow > timeToRead) // Is it time to read the answer of our AskCurrentEpoch?
  {
    // Yes, it is time to read the answer. 
    ReadCurrentEpoch(); // Read the server response
    timeToRead = 0; // We have read the response, so reset for next time we need to ask for time.
  }
    
  if (lastEpoch != 0) {  // If we don't have lastEpoch yet, return zero so we won't try to display millis on the clock
    unsigned long zoneOffset = String(timezone).toInt() * 3600;
    unsigned long elapsedMillis = millis() - lastEpochTimeStamp;
    currentTime =  lastEpoch + zoneOffset + (elapsedMillis / 1000);
  }
  return currentTime;
}

byte NTPClient::GetHours()
{
  int hours = (currentTime  % 86400L) / 3600;
  if (hours > 12 && military[0]=='N') hours -= 12;
  return hours;
}

byte NTPClient::GetMinutes()
{
  return (currentTime  % 3600) / 60;
}

byte NTPClient::GetSeconds()
{
  return currentTime % 60;
}

void NTPClient::PrintTime()
{
  if (DEBUG) 
  {
    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    byte hh = GetHours();
    byte mm = GetMinutes();
    byte ss = GetSeconds();

    Serial.print(hh); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if ( mm < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print(mm); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ( ss < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(ss); // print the second
  }
}

