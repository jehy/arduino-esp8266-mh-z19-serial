
#define INTERVAL 5000
#define DHT_PIN D5
#define DHT_VERSION DHT22
#define MH_Z19_RX D7
#define MH_Z19_TX D6
#define WIFI_MAX_ATTEMPTS_INIT 3 //set to 0 for unlimited, do not use more then 65535
#define WIFI_MAX_ATTEMPTS_SEND 1 //set to 0 for unlimited, do not use more then 65535
#define MAX_DATA_ERRORS 15 //max of errors, reset after them
#define USE_GOOGLE_DNS true

#include <SoftwareSerial.h>
#include <DHT.h> // https://github.com/adafruit/DHT-sensor-library
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <LiquidCrystal_I2C.h> // https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include "WiFiUtils.h"
#include "WiFiCreds.h"
#include "LcdPrint.h"
#include "dataServer.h"


long previousMillis = 0;
int errorCount = 0;
WiFiClient client;
WiFiUtils wifiUtils;
lcdPrint lcd(0x3F, 16, 2); // display address and size

DHT dht(DHT_PIN, DHT_VERSION);//define temperature and humidity sensor
SoftwareSerial co2Serial(MH_Z19_RX, MH_Z19_TX); // define MH-Z19

void(* resetFunc) (void) = 0; //declare reset function @ address 0


int readCO2()
{

  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  // command to ask for data
  byte response[9]; // for answer

  co2Serial.write(cmd, 9); //request PPM CO2

  // The serial stream can get out of sync. The response starts with 0xff, try to resync.
  while (co2Serial.available() > 0 && (unsigned char)co2Serial.peek() != 0xFF) {
    co2Serial.read();
  }

  memset(response, 0, 9);
  co2Serial.readBytes(response, 9);

  if (response[1] != 0x86)
  {
    Serial.println("Invalid response from co2 sensor!");
    return -1;
  }

  byte crc = 0;
  for (int i = 1; i < 8; i++) {
    crc += response[i];
  }
  crc = 255 - crc + 1;

  if (response[8] == crc) {
    int responseHigh = (int) response[2];
    int responseLow = (int) response[3];
    int ppm = (256 * responseHigh) + responseLow;
    return ppm;
  } else {
    Serial.println("CRC error!");
    return -1;
  }
}

bool sendData(JsonObject& root)
{
  Serial.println("Starting connection to server...");
  bool res = false;
  if (client.connect(DATA_SERVER, 80))
  {
    Serial.println("connected to server");
    // Make a HTTP request:
    /*client.println("GET /send.php?data={\"id\":1,\"temp\":" + String(t) + ",\"humidity\":" + String(h) + ",\"ppm\":" + String((int)ppm) +
                   ",\"mac\":\"" + String(macStr) + "\",\"FreeRAM\":\"" + String(mem) + "\",\"SSID\":\"" + WiFi.SSID() + "\"} HTTP/1.1");
    */
    client.println("POST " + String(DATA_URL) + " HTTP/1.1");
    client.println("Host: " + String(DATA_SERVER));
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
      res = true;//TODO: check if reply is really OK
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



void setup() {
  Serial.begin(115200); // Init console
  Serial.println("Setup started");

  unsigned long previousMillis = millis();
  co2Serial.begin(9600); //Init sensor MH-Z19(14)
  dht.begin();


  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true)
      delay(1000);
  }
  WiFi.mode(WIFI_STA);//be only wifi client, not station

  lcd.init();
  lcd.backlight();
  lcd.printLine(1, "Connecting...");

  // attempt to connect to Wifi network:
  unsigned int attempt = 0;
  while ( WiFi.status() != WL_CONNECTED) {
    if (WIFI_MAX_ATTEMPTS_INIT != 0 && attempt > WIFI_MAX_ATTEMPTS_INIT)
      break;
    if (attempt >= 65535)
      attempt = 0;
    attempt++;
    lcd.printLine(2, "Attempt " + String(attempt));
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    WiFi.begin(ssid, pass);
    delay(10000);
  }

  if (USE_GOOGLE_DNS)
    wifiUtils.setGoogleDNS();
  // you're connected now, so print out the data:
  Serial.println("Connected to network");
  wifiUtils.printCurrentNet();
  wifiUtils.printWifiData();

  Serial.println("Waiting for sensors to init");

  lcd.printLine(1, "Heating...");
  lcd.printLine(2, "");

  while (millis() - previousMillis < 10000)
    delay(1000);
  Serial.println("Setup finished");
  Serial.println("");
  lcd.printLine(1, "Heating...");
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
    lcd.printLine(1, "Too many errors.");
    lcd.printLine(2, "Trying to reset.");
    lcd.backlight();
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
    dataError = true;
    lcd.printParam(2, "PPM err");
    lcd.noBacklight();
  }
  else
  {
    lcd.printParam(2, "PPM=" + String(ppm));
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
    lcd.printParam(3, "T err");
    dataError = true;
  }
  else {
    lcd.printParam(3, "T=" + String(t) + (char)223);
  }
  if (h > 100 || h == 0)
  {
    Serial.println("Humidity not valid");
    lcd.printParam(1, "H err");
    dataError = true;
  }
  else {
    lcd.printParam(1, "H=" + String(h) + "%");
  }
  if (dataError)
  {
    lcd.printParam(4, "WiFi skp");
    Serial.println("Skipping loop");
    errorCount++;
    return;
  }
  errorCount = 0;

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["id"] = DATA_SENSOR_ID;
  root["temp"] = t;
  root["humidity"] = h;
  root["ppm"] = ppm;
  root["mac"] = wifiUtils.macStr();
  root["FreeRAM"] = mem;
  root["SSID"] = WiFi.SSID();


  wifiUtils.checkReconnect(ssid, pass, WIFI_MAX_ATTEMPTS_SEND);
  if (USE_GOOGLE_DNS)
    wifiUtils.setGoogleDNS();
  wifiUtils.printCurrentNet();

  lcd.printParam(4, "WiFi *");
  bool sentOk = sendData(root);
  Serial.println("Request sent");
  if (sentOk)
    lcd.printParam(4, "WiFi ok!");
  else
    lcd.printParam(4, "WiFi err");
  Serial.println("loop finished");
  Serial.println("");
}
