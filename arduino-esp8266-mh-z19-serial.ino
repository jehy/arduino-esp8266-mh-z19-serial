#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include "settings.h"
#include "LcdPrint.h"
#include "WiFiUtils.h"
#include "MHZ19.h"

long previousMillis = 0;
long previousMillisSend = 0;
int errorCount = 0;
WiFiClient client;
PubSubClient mqttClient(client);
MHZ19 co2Sensor(MH_Z19_RX, MH_Z19_TX);
lcdPrint lcd(0x3F, 16, 2); // display address and size
DHT dht(DHT_PIN, DHT_VERSION);                  //define temperature and humidity sensor

void (*resetFunc)(void) = 0; //declare reset function @ address 0

bool sendData(DynamicJsonDocument root)
{
  Serial.println("Starting connection to server...");
  if (!client.connect(DATA_SERVER, 80))
  {
    Serial.println("Failed to connect to server");
    return false;
  }

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
  serializeJson(root, data);

  //Serial.println("data to send:");
  //Serial.println(data);
  data = "data=" + data;
  client.println(data.length());
  client.println();
  client.println(data);
  while (client.connected() && !client.available()) //see https://github.com/esp8266/Arduino/issues/4342
  {
    delay(100);
  }
  Serial.println("Server reply:");
  Serial.println("");
  while (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }
  client.stop();
  return true;
}

void setup()
{
  Serial.begin(115200); // Init console
  Serial.println("Setup started");

  unsigned long previousMillis = millis();
  co2Sensor.start(); //Init sensor MH-Z19(14)
  dht.begin();

  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present");
  }
  WiFi.mode(WIFI_STA); //be only wifi client, not station
  WiFiUtils::printNetworks();
  WiFi.hostname("CO2_Sensor");

  lcd.init();
  lcd.backlight();
  lcd.printLine(1, "Connecting...");

  // attempt to connect to Wifi network:
  unsigned int attempt = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    if (WIFI_MAX_ATTEMPTS_INIT != 0 && attempt > WIFI_MAX_ATTEMPTS_INIT)
      break;
    if (attempt >= 65535)
      attempt = 0;
    attempt++;
    lcd.printLine(2, "Attempt " + String(attempt));
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_SSID);
    // Connect to WPA/WPA2 network:
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    delay(10000);
  }

  if (USE_GOOGLE_DNS)
    WiFiUtils::setGoogleDNS();
  WiFiUtils::printWiFiStatus();
  WiFiUtils::printCurrentNet();
  WiFiUtils::printWifiData();

  if (MQTT_ENABLED) {
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  }
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
  if (CHECK_INTERVAL > (currentMillis - previousMillis))
  {
    delay(100);
    return;
  }
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
  int ppm = co2Sensor.read();
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

  if (t < 5 || t > 80)
  {
    Serial.println("Temperature not valid");
    lcd.printParam(3, "T err");
    dataError = true;
  }
  else
  {
    lcd.printParam(3, "T=" + String(t) + (char)223);
  }
  if (h > 100 || h == 0)
  {
    Serial.println("Humidity not valid");
    lcd.printParam(1, "H err");
    dataError = true;
  }
  else
  {
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

  unsigned long currentMillisSend = millis();
  if (WIFI_SEND_INTERVAL > (currentMillisSend - previousMillisSend)) {
    Serial.println("Not sending to server - too early");
    return;
  }
  previousMillisSend = currentMillisSend;

  WiFiUtils::checkReconnect(WIFI_SSID, WIFI_PASS, WIFI_MAX_ATTEMPTS_SEND);
  if (USE_GOOGLE_DNS)
    WiFiUtils::setGoogleDNS();
  WiFiUtils::printCurrentNet();

  lcd.printParam(4, "WiFi *");
  bool sentOk = false;

  if (SERVER_ENABLED)
  {
    DynamicJsonDocument root(200);
    char macString[20];
    WiFiUtils::macStr(macString);
    root["id"] = DATA_SENSOR_ID;
    root["temp"] = t;
    root["humidity"] = h;
    root["ppm"] = ppm;
    root["mac"] = macString;
    root["FreeRAM"] = mem;
    root["SSID"] = WiFi.SSID();
    sentOk = sendData(root);
    if (sentOk) {
      Serial.println("Server request sent");
    }
  }
  if (MQTT_ENABLED) {
    bool connected = true;
    if (!mqttClient.connected()) {
      connected = false;
      if (mqttClient.connect("ESP8266Client")) {
        Serial.println("MQTT reconnected");
        connected = true;
      } else {
        connected = false;
        Serial.print("MQTT failed, rc=");
        Serial.print(mqttClient.state());
      }
    }
    if (connected) {
      char dataString[5];       // number of digits + 1 for null terminator
      itoa(t, dataString, 10);  // int value, pointer to string, base number
      mqttClient.publish(TEMPERATURE_TOPIC, dataString, true);
      itoa(h, dataString, 10);  // int value, pointer to string, base number
      mqttClient.publish(HUMIDITY_TOPIC, dataString, true);
      itoa(ppm, dataString, 10);  // int value, pointer to string, base number
      mqttClient.publish(CO2_TOPIC, dataString, true);
      itoa(mem, dataString, 10);  // int value, pointer to string, base number
      mqttClient.publish(RAM_TOPIC, dataString, true);
      Serial.println("MQTT request sent");
      sentOk = true;
    }
    else {
      Serial.println("MQTT not connected");
    }
  }
  if (sentOk)
    lcd.printParam(4, "WiFi ok!");
  else
    lcd.printParam(4, "WiFi err");
  Serial.println("loop finished");
  Serial.println("");
}
