#include "SoftwareSerial.h"
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <Wire.h>
#include "wifi-utils.h"
#include "wifi-creds.h"


char macStr[20];
long previousMillis = 0;        // will store last time LED was updated
long interval = 5000;           // interval at which to blink (milliseconds)

DHT dht(D4, DHT11);           // Объявление переменной класса dht11


SoftwareSerial mySerial(D7,D6); // RX, TX сенсора

byte cmd[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
// волшебная комманда - смотри мануал
char response[9]; // здесь будет ответ


void setup() {
Serial.begin(115200); //это наш монитор
mySerial.begin(9600); //а это датчик MH-Z19(14)
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

  
    byte mac[6];  
    WiFi.macAddress(mac);
    sprintf(macStr,"%02x:%02x:%02x:%02x:%02x:%02x",mac[5],mac[4],mac[3],mac[2],mac[1],mac[0]);
delay(2000);
}

void loop()
{
  
    unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis < interval) 
    return;
   // save the last time you blinked the LED 
   previousMillis = currentMillis;   
  //dns.processNextRequest();  
  //server.handleClient();
  Serial.println("loop started");  
  Serial.println("reading data");
mySerial.write(cmd,9);//запрос PPM CO2
mySerial.readBytes(response, 9);
int responseHigh = (int) response[2];
int responseLow = (int) response[3];
int ppm = (256*responseHigh)+responseLow;
// ну и по мануалу из ответа считаем PPM

Serial.println("PPM = "+String(ppm));



float h = dht.readHumidity();

// Считываем температуру

float t = dht.readTemperature();

  // Выводим показания влажности и температуры
  Serial.print("Humidity = ");
  Serial.print(h, 1);
  Serial.print(", Temp = ");
  Serial.println(t,1);
  
Serial.println("");


  wifiCheckReconnect(ssid,pass);
  Serial.println("\nCurrent net:");
  printCurrentNet();
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  wdt_reset();
  if (client.connect("co2.jehy.ru", 80)) 
  {
    Serial.println("connected to server");  
    // Make a HTTP request:
    client.println("GET /send.php?data={\"id\":1,\"temp\":"+String(t)+",\"humidity\":"+String(h)+",\"ppm\":"+String((int)ppm)+
    ",\"mac\":\""+String(macStr)+"\",\"SSID\":\""+WiFi.SSID()+"\"} HTTP/1.1");
    client.println("Host: co2.jehy.ru");
    client.println("Connection: close");
    client.println();
    client.stop();
    Serial.println("Request sent");
  }
  Serial.println("loop finished");
delay(2000);
}
