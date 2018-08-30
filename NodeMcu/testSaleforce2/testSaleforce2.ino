#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

boolean isSetupDebug=false;
boolean isResetWifiSettings=false;

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
String DeviceId="12312312312312312312"; // Should be restricted to 20 characters and Numeric
char shaFingerPrint[]="D1 A3 3C D7 D5 87 0A 10 81 22 BF 44 12 B8 C8 7B 1A D2 DC 94";
// -------------------------------- AUTO CONNECT DEVICE ID --------------------------------------------//
void setup()
{
  Serial.begin(115200);
  if(isSetupDebug==true){Serial.println("[LOC] Connecting to Wifi");}
  WiFiManager wifiManager;
  if(isResetWifiSettings){wifiManager.resetSettings();}
  wifiManager.setDebugOutput(isSetupDebug);
  wifiManager.autoConnect(AccessPointName, AccessPointPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  if(isSetupDebug==true){Serial.println("[LOC] Connected To Wifi");}
  boolean authenticated=false;
  while(authenticated==false)
  {
  if(isSetupDebug==true){Serial.println("[LOC] Attempting to authenticate and connect to server");}
  delay(1000); 
   authenticated= ConnectToCloud();
  }
}

boolean ConnectToCloud()
{
  HTTPClient http;
  char payload[400];
  char requestURL[100];
  String request=dateApiName+apiKey+zonePefix+zone;
  request.toCharArray(requestURL,request.length()+1);
  http.begin(requestURL);
  int httpCode1 = http.GET();
  if (httpCode1 > 0) {
    if(isSetupDebug==true){Serial.printf("[LOC] Connecting to Time API : %d\n", httpCode1);}
    if (httpCode1 == HTTP_CODE_OK) {
      String rawData = http.getString();
      rawData.toCharArray(payload, rawData.length() + 1);
    }
  } else {
    if(isSetupDebug==true){Serial.printf("[LOC] Unable to Connect to Time API :, error: %s\n", http.errorToString(httpCode1).c_str());}
    return false;
  }
  http.end();

  if(isSetupDebug==true){Serial.printf("[LOC] Date Time Data %s \n",payload);}
  String dataToken=null;
 JsonObject &root = json_buf.parseObject(payload);
  if (!root.success()) {
    if(isSetupDebug==true){Serial.println("[LOC] Parse time data failed");}
    return false;
  }
  else
  {
  String timestamp= root["timestamp"];
  if(isSetupDebug==true){Serial.println("[LOC] Retrieved Timestamp "+timestamp);}
  // Logic to generate the master token
  String dataToken = DeviceId+timestamp;
  dataToken.replace("0","s");
  dataToken.replace("1","u");
  dataToken.replace("2","c");
  dataToken.replace("3","h");
  dataToken.replace("4","a");
  dataToken.replace("5","n");
  dataToken.replace("6","d");
  dataToken.replace("7","r");
  dataToken.replace("8","p");
  dataToken.replace("9","l");
  }
  
  if(isSetupDebug==true){Serial.print("[LOC] Connecting To Server \n");}
  http.begin("https://homeautomationalpha-developer-edition.ap5.force.com/services/apexrest/iotservice?DeviceId=1231231231242312312", "D1 A3 3C D7 D5 87 0A 10 81 22 BF 44 12 B8 C8 7B 1A D2 DC 94");
  int httpCode = http.GET();
  if (httpCode > 0) {
    if(isSetupDebug==true){Serial.printf("[LOC] Connection To Server Successful : %d\n", httpCode);}
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      if(isSetupDebug==true){Serial.println("[LOC] "+payload);}
    }
  } else {
    if(isSetupDebug==true){Serial.printf("[LOC] Failed to connect to server, error: %s\n", http.errorToString(httpCode).c_str());}
    return false;
  }
  http.end();
  if(isSetupDebug==true){Serial.println("[LOC] ConnectToCloud Successful");}
  delay(1000);
  return true;
}

void loop() {
  Serial.println("hi");
  delay(90000);
}


