# arduino-esp8266-mh-z19-serial
CO2, humidity and temperature sensor on ESP8266 with mh-z19 co2 sensor and dht-22 temperature and humidity sensor.

You can use project https://github.com/jehy/co2-online-display as backend to this program.
See full description on https://geektimes.ru/post/270958/ (you can use Google translate from Russian).

# Compilation
1. Copy file `dataServer.h.sample` to `dataServer.h` and specify there your server address.
2. Copy file `WiFiCreds.h.sample` to `WiFiCreds.h` and specify there your WiFi cridentials.
3. Add `http://arduino.esp8266.com/stable/package_esp8266com_index.json` to additional boards in preferences
4. Install DHT sensor library
5. Install ArduinoJson library
6. Install LiquidCrystal_I2C library
7. Install Adafruit unified sensor library (required for DHT library)
8. Select wemos D1 (or your other board) in boards manager and install it
9. Compile and upload your sketch
10. Enjoy!
