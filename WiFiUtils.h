class WiFiUtils
{
  public:
    static void macStr(char* buffer)
    {
      byte mac[6];
      WiFi.macAddress(mac);
      sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
    }
    static void setGoogleDNS()
    {
      IPAddress googleDNS(8, 8, 8, 8);
      if (!ipCmp(WiFi.dnsIP(), googleDNS))
        WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), googleDNS);
    }
    static bool ipCmp(IPAddress ip1, IPAddress ip2)
    {
      for (int i = 0; i < 4; i++)
        if (ip1[i] != ip2[i])
          return false;
      return true;
    }
    static void checkReconnect(char *ssid, char *pass, int max_attempts = 0)
    {
      long previousMillis = 0;
      long interval = 10000;
      unsigned long currentMillis = millis();
      unsigned int attempt = 0;
      // attempt to connect to Wifi network:
      while (WiFi.status() != WL_CONNECTED)
      {
        if (max_attempts != 0 && attempt > max_attempts)
          break;
        if (attempt >= 65535)
          attempt = 0;
        attempt++;
        wdt_reset();
        if (currentMillis - previousMillis < interval)
          continue;
        previousMillis = currentMillis;
        Serial.print("Wifi not connected. Attempting to connect to SSID ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network:
        WiFi.begin(ssid, pass);
      }
    }

    static void printWifiData()
    {
      // print your WiFi shield's IP address:
      IPAddress ip = WiFi.localIP();
      Serial.print("  IP Address: ");
      Serial.println(ip);

      // print your MAC address:
      byte mac[6];
      WiFi.macAddress(mac);
      Serial.print("  MAC address: ");
      Serial.print(mac[5], HEX);
      Serial.print(":");
      Serial.print(mac[4], HEX);
      Serial.print(":");
      Serial.print(mac[3], HEX);
      Serial.print(":");
      Serial.print(mac[2], HEX);
      Serial.print(":");
      Serial.print(mac[1], HEX);
      Serial.print(":");
      Serial.println(mac[0], HEX);
    }

    static void printCurrentNet()
    {
      Serial.println("Current net:");
      // print the SSID of the network you're attached to:
      Serial.print("  SSID: ");
      Serial.println(WiFi.SSID());

      // print the received signal strength:
      long rssi = WiFi.RSSI();
      Serial.print("  signal strength (RSSI):");
      Serial.println(rssi);

      // print the encryption type:
      /*byte encryption = WiFi.encryptionType();
          Serial.print("Encryption Type:");
          Serial.println(encryption,HEX);
          Serial.println();*/
    }
    static void printWiFiStatus() {

      Serial.printf("Connection status: %d\n", WiFi.status());

      if (WiFi.status() == WL_IDLE_STATUS)
      {
        Serial.println("WL_IDLE_STATUS");
      }
      else if (WiFi.status() == WL_NO_SSID_AVAIL)
      {
        Serial.println("WL_NO_SSID_AVAIL");
      }
      else if (WiFi.status() == WL_SCAN_COMPLETED)
      {
        Serial.println("WL_SCAN_COMPLETED");
      }
      else if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("WL_CONNECTED");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
      }
      else if (WiFi.status() == WL_CONNECT_FAILED)
      {
        Serial.println("WL_CONNECT_FAILED");
      }
      else if (WiFi.status() == WL_CONNECTION_LOST)
      {
        Serial.println("WL_CONNECTION_LOST");
      }
      else if (WiFi.status() == WL_DISCONNECTED)
      {
        Serial.println("WL_DISCONNECTED");
      }
    }
    static void printNetworks() {
      Serial.print("Scan start ... ");
      int n = WiFi.scanNetworks();
      Serial.print(n);
      Serial.println(" network(s) found");
      for (int i = 0; i < n; i++)
      {
        Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
      }
      Serial.println();
      WiFi.disconnect(1);
    }
};
