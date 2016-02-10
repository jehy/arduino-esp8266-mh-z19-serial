#include <SoftwareSerial.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include "wifi-utils.h"
#include "wifi-creds.h"

#define INTERVAL 5000
#define DHT_PIN D5
#define DHT_VERSION DHT11
#define MH_Z19_RX D7
#define MH_Z19_TX D6
#define WIFI_MAX_ATTEMPTS_INIT 5 //set to 0 for unlimited, do not use more then 65535
#define WIFI_MAX_ATTEMPTS_SEND 1 //set to 0 for unlimited, do not use more then 65535
#define MAX_DATA_ERRORS 15 //max of errors, reset after them

char macStr[20];
long previousMillis = 0;
int errorCount = 0;

LiquidCrystal_I2C lcd(0x3F, 16, 2); // display address and size
DHT dht(DHT_PIN, DHT_VERSION);//define temperature and humidity sensor


SoftwareSerial mySerial(MH_Z19_RX, MH_Z19_TX); // define MH-Z19 using

void(* resetFunc) (void) = 0; //declare reset function @ address 0

int readCO2()
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

bool sendData(JsonObject& root)
{
  Serial.println("Starting connection to server...");
  bool res = false;
  if (client.connect("co2.jehy.ru", 80))
  {
    Serial.println("connected to server");
    // Make a HTTP request:
    /*client.println("GET /send.php?data={\"id\":1,\"temp\":" + String(t) + ",\"humidity\":" + String(h) + ",\"ppm\":" + String((int)ppm) +
                   ",\"mac\":\"" + String(macStr) + "\",\"FreeRAM\":\"" + String(mem) + "\",\"SSID\":\"" + WiFi.SSID() + "\"} HTTP/1.1");
    */
    client.println("POST /send.php HTTP/1.1");
    client.println("Host: co2.jehy.ru");
    client.println("Connection: close");
    client.println("User-Agent: Arduino/1.0");
    client.println("Content-Type: application/x-www-form-urlencoded;");

    client.print("Content-Length: ");
    String data;
    root.printTo(data);

    //Serial.println("data to send:");
    //Serial.println(data);
    data = "data=" + data;
    client.println(data.length());
    client.println();
    client.println(data);

    if (client.available())
    {
      res = true;
      Serial.println("Server reply:");
      Serial.println("");
      while (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
    }
    client.stop();
    return res;
  }
  else
  {
    Serial.println("Failed to connect to server");
    return false;
  }
}

void lcdPrint(int statNum, String str)
{
  if (statNum == 1)
    lcd.setCursor(0, 0);
  else if (statNum == 2)
    lcd.setCursor(8, 0);
  else if (statNum == 3)
    lcd.setCursor(0, 1);
  else if (statNum == 4)
    lcd.setCursor(8, 1);
  while (str.length() < 8)
    str += " ";
  lcd.print(str);
}


void setup() {
  Serial.begin(115200); // Init console
  Serial.println("Setup started");
  mySerial.begin(9600); //Init sensor MH-Z19(14)
  dht.begin();

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true)
      delay(10000);
  }


  // attempt to connect to Wifi network:
  unsigned int attempt = 0;
  while ( status != WL_CONNECTED) {
    if (WIFI_MAX_ATTEMPTS_INIT != 0 && attempt > WIFI_MAX_ATTEMPTS_INIT)
      break;
    if (attempt >= 65535)
      attempt = 0;
    attempt++;
    lcd.setCursor(0, 0);
    lcd.print("Connecting " + String(attempt) + "...");
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  //set google DNS
  IPAddress googleDNS(8, 8, 8, 8);
  WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), googleDNS);

  // you're connected now, so print out the data:
  Serial.println("Connected to network");
  printCurrentNet();
  printWifiData();

  //get MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(macStr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);

  Serial.println("Waiting for sensors to init");
  lcd.setCursor(0, 0);
  lcd.print("Heating......");
  delay(10000);
  Serial.println("Setup finished");
  Serial.println("");
  lcd.setCursor(0, 0);
  lcd.print("Starting.....");
  lcd.noBacklight();
}

void loop()
{

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis < INTERVAL)
    return;
  previousMillis = currentMillis;
  Serial.println("loop started");

  if (errorCount > MAX_DATA_ERRORS)
  {
    Serial.println("Too many errors, resetting");
    lcdPrint(1, "Too many errors.");
    lcdPrint(3, "Trying to reset.");
    delay(2000);
    resetFunc();
  }
  Serial.println("reading data:");
  int ppm = readCO2();
  bool dataError = false;
  Serial.println("  PPM = " + String(ppm));

  if (ppm < 100 || ppm > 6000)
  {
    Serial.println("PPM not valid");
    lcdPrint(2, "PPM err");
    dataError = true;
    lcd.noBacklight();
  }
  else
  {
    lcdPrint(2, "PPM=" + String(ppm));
    if (ppm >= 1000)
      lcd.backlight();
    else
      lcd.noBacklight();
  }
  int mem = ESP.getFreeHeap();
  Serial.println("  Free RAM: " + String(mem));

  int h = dht.readHumidity();
  int t = dht.readTemperature();

  Serial.print("  Humidity = ");
  Serial.print(h, 1);
  Serial.print(", Temp = ");
  Serial.println(t, 1);

  if (t < 5 || t > 80 )
  {
    Serial.println("Temperature not valid");
    lcdPrint(3, "T err");
    dataError = true;
  }
  else
    lcdPrint(3, "T=" + String(t) + (char)223);

  if (h > 100 || h == 0)
  {
    Serial.println("Humidity not valid");
    lcdPrint(1, "H err");
    dataError = true;
  }
  else
    lcdPrint(1, "H=" + String(h) + "%");

  if (dataError)
  {
    lcdPrint(4, "WiFi skp");
    Serial.println("Skipping loop");
    errorCount++;
    return;
  }
  errorCount = 0;

  wdt_reset();
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["id"] = 1;
  root["temp"] = t;
  root["humidity"] = h;
  root["ppm"] = ppm;
  root["mac"] = macStr;
  root["FreeRAM"] = mem;
  root["SSID"] = WiFi.SSID();
  root["FreeRAM"] = mem;


  wifiCheckReconnect(ssid, pass, WIFI_MAX_ATTEMPTS_SEND);
  printCurrentNet();

  lcdPrint(4, "WiFi *");
  bool sentOk = sendData(root);
  Serial.println("Request sent");
  if (sentOk)
    lcdPrint(4, "WiFi ok!");
  else
    lcdPrint(4, "WiFi err");
  Serial.println("loop finished");
  Serial.println("");
}
