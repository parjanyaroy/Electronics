#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>

// -------------------------------- TIME ZONE AUTHENTICATION DATA --------------------------------------------//
char dateApiName[]="http://api.timezonedb.com/v2/get-time-zone";
char apiKey[]="RKEX6GHOJMTH";
char zone[]="Asia/Kolkata";
// -------------------------------- TIME ZONE AUTHENTICATION DATA --------------------------------------------//


// -------------------------------- SALESFORCE SERVER DATA --------------------------------------------//
char servername[]="homeautomationalpha-developer-edition.ap4.force.com";              // remote server we will connect to
// -------------------------------- SALESFORCE SERVER DATA --------------------------------------------//

// -------------------------------- AUTO CONNECT MANAGER DATA --------------------------------------------//
char AccessPointName[]="Home Automation Setup";
char AccessPointPassword[]="Home@1234";
// -------------------------------- AUTO CONNECT MANAGER DATA --------------------------------------------//


String result;

int  counter = 60;                                      

const char* server = "homeautomationalpha-developer-edition.ap4.force.com";
const char* _getLink = "http://homeautomationalpha-developer-edition.ap4.force.com/services/apexrest/Widgets1";
 void setup() 
 {
  Serial.begin(115200);
  Serial.println("Connecting");
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  //wifiManager.setDebugOutput(false);
  wifiManager.autoConnect(AccessPointName,AccessPointPassword);
  while (WiFi.status() != WL_CONNECTED){delay(500);}
  Serial.println("Connected");
  delay(1000);
  ConnectToCloud();
}

void ConnectToCloud()
{
   HTTPClient http;
    //http.begin(dateApiName.."?key="..apiKey+"&format=json&by=zone&zone="..zone);
    http.begin("http://api.timezonedb.com/v2/get-time-zone?key=RKEX6GHOJMTH&format=json&by=zone&zone=Asia/Kolkata");
    int httpCode1 = http.GET();
    if (httpCode1 > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode1);
      if (httpCode1 == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode1).c_str());
    }
    http.end();

    
    Serial.print("[HTTP] begin...\n"); 
    http.begin("https://homeautomationalpha-developer-edition.ap5.force.com/services/apexrest/iotservice?DeviceId=1231231231242312312","D1 A3 3C D7 D5 87 0A 10 81 22 BF 44 12 B8 C8 7B 1A D2 DC 94");
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
Serial.print("hi");
  }


