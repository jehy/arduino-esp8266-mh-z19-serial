# Arduino esp8266 mh-z19 serial

[![Build Status](https://travis-ci.com/jehy/arduino-esp8266-mh-z19-serial.svg?branch=master)](https://travis-ci.com/jehy/arduino-esp8266-mh-z19-serial)
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/jehyrus)
[![License](https://img.shields.io/badge/License-BSD%202--Clause-orange.svg)](https://opensource.org/licenses/BSD-2-Clause)

CO2, humidity and temperature sensor on ESP8266 with mh-z19 co2 sensor and dht-22 temperature and humidity sensor.

You can either use custom backed server to receive data in JSON or build in MQQT client to publish data to MQQT server.

![sensor sample](https://github.com/jehy/arduino-esp8266-mh-z19-serial/raw/master/sensor.jpg)

You can use project [https://github.com/jehy/co2-online-display](https://github.com/jehy/co2-online-display) as backend to this program.
See full description on [https://geektimes.ru/post/270958/](https://geektimes.ru/post/270958/) (you can use Google translate from Russian).

# Compilation

All required libraries can be installed with Arduino IDE:

## Via console:

```bash
arduino --pref "boardsmanager.additional.urls=http://arduino.esp8266.com/stable/package_esp8266com_index.json" --save-prefs
arduino --install-library "ArduinoJson:6.14.1"
arduino --install-library "Adafruit Unified Sensor:1.1.2"
arduino --install-library "DHT sensor library:1.3.8"
arduino --install-library "LiquidCrystal I2C:1.1.2"
arduino --install-library "PubSubClient:2.8.0"
arduino --install-boards esp8266:esp8266

cp settings.sample.h settings.h # EDIT AFTER COPY!

arduino --verify --board esp8266:esp8266:d1:CpuFrequency=80,FlashSize=4M3M arduino-esp8266-mh-z19-serial.ino

```

## Manualy:

1. Copy file `settings.sample.h` to `settings.h` and specify your settings;
2. Install [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor) version 1.1.2;
3. Install [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) version 1.3.8; 
4. Install [ArduinoJson library](https://arduinojson.org/) v 6.14.1;
5. Install [LiquidCrystal_I2C library](https://github.com/marcoschwartz/LiquidCrystal_I2C) v 1.1.2;
6. Compile and upload your sketch;
7. Enjoy!

For Wemos D1, before compiling sketch:

1. Add `http://arduino.esp8266.com/stable/package_esp8266com_index.json` to additional boards in preferences;
2. Select wemos D1 (or your other board) in boards manager and install it.
