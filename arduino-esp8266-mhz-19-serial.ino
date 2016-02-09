#include "SoftwareSerial.h"
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include "wifi-utils.h"
#include "wifi-creds.h"


#define INTERVAL 5000
#define DHT_PIN D4
#define DHT_VERSION DHT11
#define MH_Z19_RX D7
#define MH_Z19_TX D6

char macStr[20];
long previousMillis = 0;

DHT dht(DHT_PIN, DHT_VERSION);//define temperature and humidity sensor


SoftwareSerial mySerial(MH_Z19_RX, MH_Z19_TX); // define MH-Z19 using



void setup() {
  Serial.begin(115200); // Init console
  mySerial.begin(9600); //Init sensor MH-Z19(14)
  dht.begin();

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true)
      delay(10000);
  }


  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

  //get MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(macStr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  
  delay(10000);//to init and heat sensors
}

int ReadCO2()
{

  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  // command to ask for data
  char response[9]; // for answer

  mySerial.write(cmd, 9); //request PPM CO2
  mySerial.readBytes(response, 9);
  int responseHigh = (int) response[2];
  int responseLow = (int) response[3];
  int ppm = (256 * responseHigh) + responseLow;
  return ppm;
}
void loop()
{

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis < INTERVAL)
    return;
  previousMillis = currentMillis;
  //dns.processNextRequest();
  //server.handleClient();
  Serial.println("loop started");
  Serial.println("reading data");
  int ppm = ReadCO2();

  Serial.println("PPM = " + String(ppm));

  int mem = ESP.getFreeHeap();
  Serial.println("Free RAM: " + String(mem));

  float h = dht.readHumidity();

  float t = dht.readTemperature();

  Serial.print("Humidity = ");
  Serial.print(h, 1);
  Serial.print(", Temp = ");
  Serial.println(t, 1);

  wifiCheckReconnect(ssid, pass);
  Serial.println("Current net:");
  printCurrentNet();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial:
  wdt_reset();
  if (client.connect("co2.jehy.ru", 80))
  {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /send.php?data={\"id\":1,\"temp\":" + String(t) + ",\"humidity\":" + String(h) + ",\"ppm\":" + String((int)ppm) +
                   ",\"mac\":\"" + String(macStr) + "\",\"FreeRam\":\"" + String(mem) + "\",\"SSID\":\"" + WiFi.SSID() + "\"} HTTP/1.1");
    client.println("Host: co2.jehy.ru");
    client.println("Connection: close");
    client.println();
    client.stop();
    Serial.println("Request sent");
  }
  Serial.println("loop finished");
  Serial.println("");
  delay(2000);
}
