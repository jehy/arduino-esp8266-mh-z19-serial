# Arduino esp8266 mh-z19 serial

CO2, humidity and temperature sensor on ESP8266 with mh-z19 co2 sensor and dht-22 temperature and humidity sensor.

You can use project https://github.com/jehy/co2-online-display as backend to this program.
See full description on https://geektimes.ru/post/270958/ (you can use Google translate from Russian).

# Compilation

All required libraries can be installed with Arduino IDE.
 
1. Copy file `dataServer.h.sample` to `dataServer.h` and specify there your server address;
2. Copy file `WiFiCreds.h.sample` to `WiFiCreds.h` and specify there your WiFi cridentials;
3. Install [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) version 1.2.3; 
4. Install [ArduinoJson library](https://arduinojson.org/) v 5.13.3;
5. Install [LiquidCrystal_I2C library](https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library) v 1.1.2;
6. Install [Adafruit unified sensor library](https://github.com/adafruit/Adafruit_Sensor) (required for DHT library);
7. Compile and upload your sketch;
8. Enjoy!

For Wemos D1, before compiling sketch:

1. Add `http://arduino.esp8266.com/stable/package_esp8266com_index.json` to additional boards in preferences;
2. Select wemos D1 (or your other board) in boards manager and install it.
