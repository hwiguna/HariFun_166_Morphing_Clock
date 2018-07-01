#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "NTPClient.h"

#define DEBUG 0

unsigned int localPort = 2390;      // local port to listen for UDP packets

/* Don't hardwire the IP address or we won't get the benefits of the pool.
    Lookup the IP address for the host name instead */
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

#include "C:\Users\hwiguna\Documents\Arduino\WiFiCredentials.h"
//#include "C:\Users\hwiguna\Documents\Arduino\MakeShiftWiFiCredentials.h"
char ssid[] = WLAN_SSID;  //  your network SSID (name)
char pass[] = WLAN_PASS;       // your network password

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

unsigned long askFrequency = 300000; // How frequent should we get current time? in miliseconds
unsigned long timeToAsk;
unsigned long timeToRead;
unsigned long epoch;
unsigned long epochTimeStamp; // What was millis() when we received last epoch?
unsigned long currentTime;

NTPClient::NTPClient()
{
}

void NTPClient::Setup()
{
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
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
    epochTimeStamp = millis();
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
    epoch = secsSince1900 - seventyYears; // 1530082740; // Fake 6:59:00
    // print Unix time:
    if (DEBUG) Serial.println(epoch);

    return epoch;
  }
}

unsigned long NTPClient::GetCurrentTime()
{
  if (DEBUG) Serial.println("GetCurrentEpoch called");
  if (millis() > timeToAsk) { // Is it time to ask server for current time?
    if (timeToRead == 0) { // Yes. Have we asked yet?
      ReadCurrentEpoch(); // Flush out any remaining answers from previous Asks
      AskCurrentEpoch(); //  No, we're not waiting to read, that meant we have not asked yet, so ask!
      timeToRead = millis() + 1000; // Now that we've asked, wait a second to read the answer.
    } else if (millis() > timeToRead) // Is it time to read the answer of our AskCurrentEpoch?
    {
      // Yes, it is time to read the answer. Read it!
      ReadCurrentEpoch();
      timeToRead=0;
      timeToAsk = millis() + askFrequency;
    }
  }

  unsigned long elapsedMillis = millis() - epochTimeStamp;
  currentTime = (epoch==0) ? 0 : epoch + (elapsedMillis / 1000); // If we don't have epoch yet, return zero so we won't try to display millis on the clock.
  return currentTime;
}


byte NTPClient::GetHours()
{
  return (currentTime  % 86400L) / 3600;
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
  if (DEBUG) {
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

