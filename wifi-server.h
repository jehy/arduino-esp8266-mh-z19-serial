
IPAddress apIP(192, 168, 4, 100);
DNSServer dns;
ESP8266WebServer server(80);

const byte DNS_PORT = 53;
const char *ssid = "MQ135";
const char *password = "";

void showPage() {
  uint16_t valr = analogRead(A0);
  uint16_t val =  ((float)22000*(1023-valr)/valr); 
  float mq135_ro = mq135_getro(15231, 660);//8000;//mq135_getro(val, 500);
  //convert to ppm (using default ro)
  float valAIQ = mq135_getppm(val, mq135_ro);
  float valAIQ_defro = mq135_getppm(val, MQ135_DEFAULTRO);

  Serial.println ( "Valr /Val / Ro / PPM:");
  Serial.print(valr);
  Serial.print ( " / ");
  Serial.print ( val);
  Serial.print ( " / ");
  Serial.print ( mq135_ro);
  Serial.print ( " / ");
  Serial.println ( valAIQ);
  String result="<table><tr><td>Val</td><td>Valr</td><td>Ro</td><td>PPM</td><td>PPM with def ro</td></tr><tr><td>"+
  String(val)+"</td><td>"+ String(valr)+"</td><td>"+String(mq135_ro)+"</td><td>"+String(valAIQ)+"</td><td>"+String(valAIQ_defro)+"</td></tr></table>";
  /*
  String result = "Not found";
  long ro0= mq135_getro(215,537);
  if (server.hasArg("ppm")) {
    double ppm = server.arg("ppm").toInt();
    result = "RO: " + String(mq135_getro(res, ppm)); 
  } else if (server.hasArg("ro")) {
    long ro = server.arg("ro").toInt();
    result = "PPM: " + String(mq135_getppm(res, ro));
  } else {
   result = "RAW: " + String(res)+ "PPM: " + String(mq135_getppm(res, ro0));
  }*/
  server.send(200, "text/html", "<meta http-equiv=refresh content=4>" + result);
  //delay(3000);
  //gw.sleep(1000); //sleep for: sleepTime
}

/*function setup()
{

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  
  dns.start(DNS_PORT, "*", apIP);
  
  server.onNotFound(showPage);
  server.begin();  
}*/