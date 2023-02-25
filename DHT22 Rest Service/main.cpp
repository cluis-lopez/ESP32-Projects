#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

#define DHTPIN 4

#define DHTTYPE DHT22

const char *ssid = "XXXXX";
const char *password = "YYYYYYY";

StaticJsonDocument<200>  json;
DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// Initialize WiFi
void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("Clopez's ESP32");
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("Conected to ");
  Serial.print(ssid);
  Serial.print(" with local IP ");
  Serial.println(WiFi.localIP());
}

// Read data from DHT22
String readData()
{
  String ret;
  json ["humedad"] = dht.readHumidity();
  json ["temperatura"] = dht.readTemperature(false);
  serializeJson(json, ret);
  return ret;
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

void handle_sendData(){
  server.send(200, "application/json", readData());
  Serial.println(readData());
}

void handle_mainPage(){
  File file = SPIFFS.open("/index.html", "r");
  Serial.println("Sending index.html");
  server.streamFile(file, "text/html");
  file.close();
}

void handle_stylePage(){
  File file = SPIFFS.open("/style.css", "r");
  Serial.println("Sending style.css");
  server.streamFile(file, "text/css");
  file.close();
}

void setup() {
  Serial.begin(115200);
  Serial.println("DHTxx web test!");
  initWiFi();
  dht.begin();

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
  } else {
    Serial.println("SPIFFS filesystem mounted");
  }

  server.on("/", handle_mainPage);
  server.on("/style.css", handle_stylePage);
  server.on("/data", handle_sendData);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
