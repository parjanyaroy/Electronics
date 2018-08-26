#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

// -------------------------------- TIME ZONE AUTHENTICATION DATA --------------------------------------------//
String dateApiName = "http://api.timezonedb.com/v2/get-time-zone?key=";
String apiKey = "RKEX6GHOJMTH";
String zonePefix="&format=json&by=zone&zone=";
String zone = "Asia/Kolkata";
StaticJsonBuffer<1024> json_buf;
// -------------------------------- TIME ZONE AUTHENTICATION DATA --------------------------------------------//


// -------------------------------- SALESFORCE SERVER DATA --------------------------------------------//
char servername[] = "homeautomationalpha-developer-edition.ap4.force.com";            // remote server we will connect to
// -------------------------------- SALESFORCE SERVER DATA --------------------------------------------//

// -------------------------------- AUTO CONNECT MANAGER DATA --------------------------------------------//
char AccessPointName[] = "Home Automation Setup";
char AccessPointPassword[] = "Home@1234";
// -------------------------------- AUTO CONNECT MANAGER DATA --------------------------------------------//


String result;

// -------------------------------- AUTO CONNECT DEVICE ID DATA--------------------------------------------//
String server = "https://homeautomationalpha-developer-edition.ap5.force.com/services/apexrest/iotservice?";
String DeviceId="1231231231242312312";
char shaFingerPrint[]="D1 A3 3C D7 D5 87 0A 10 81 22 BF 44 12 B8 C8 7B 1A D2 DC 94";
// -------------------------------- AUTO CONNECT DEVICE ID --------------------------------------------//
void setup()
{
  Serial.begin(115200);
  Serial.println("Connecting");
  WiFiManager wifiManager;
  //wifiManager.resetSettings();
  //wifiManager.setDebugOutput(false);
  wifiManager.autoConnect(AccessPointName, AccessPointPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Connected");
  delay(1000);
  ConnectToCloud();
}

void ConnectToCloud()
{
  HTTPClient http;
  char payload[400];
  char requestURL[100];
  String request=dateApiName+apiKey+zonePefix+zone;
  request.toCharArray(requestURL,request.length()+1);
  http.begin(requestURL);
  int httpCode1 = http.GET();
  if (httpCode1 > 0) {
    Serial.printf("[HTTP] GET Timezone : %d\n", httpCode1);
    if (httpCode1 == HTTP_CODE_OK) {
      String rawData = http.getString();
      rawData.toCharArray(payload, rawData.length() + 1);
    }
  } else {
    Serial.printf("[HTTP] GET Timezone, error: %s\n", http.errorToString(httpCode1).c_str());
  }
  http.end();


  //payload.replace('[', ' ');
  //payload.replace(']', ' ');
  Serial.println("-------PAYLOAd------");
  Serial.println(payload);

 JsonObject &root = json_buf.parseObject(payload);
  if (!root.success()) {
    Serial.println("parseObject() failed");
  }
  else
  {
  String timestamp = root["timestamp"];
  Serial.println(timestamp);
  }
  Serial.print("[HTTP] begin...\n");
  http.begin("https://homeautomationalpha-developer-edition.ap5.force.com/services/apexrest/iotservice?DeviceId=1231231231242312312", "D1 A3 3C D7 D5 87 0A 10 81 22 BF 44 12 B8 C8 7B 1A D2 DC 94");
  Serial.print("[HTTP] GET...\n");
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  delay(5000);
}

void loop() {
  Serial.println("hi");
  delay(90000);
}


