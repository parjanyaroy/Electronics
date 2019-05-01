#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <string.h>


#include <ESP8266WiFi.h>




// -------------------------------- AUTO CONNECT MANAGER DATA --------------------------------------------//
char AccessPointName[] = "Home Automation Setup";
char AccessPointPassword[] = "Home@1234";
// -------------------------------- AUTO CONNECT MANAGER DATA --------------------------------------------//

// -------------------------------- AUTO CONNECT DEVICE ID DATA--------------------------------------------//
String server = "https://sandbox-parjanyaroy-developer-edition.cs58.force.com/services/apexrest/iotservice?deviceid="; //DIRECT SFDC CONNECT
String serverURL =""; 
char shaFingerPrint[]="EF C6 00 66 2D 39 38 F8 D6 FD 80 81 63 90 43 CF 01 91 D4 79";
//char shaFingerPrint[]="08 3B 71 72 02 43 6E CA ED 42 86 93 BA 7E DF 81 C4 BC 62 30";

// -------------------------------- AUTO CONNECT DEVICE ID --------------------------------------------//


WiFiServer DeviceServer(1025);


int executingSetup= 4;
int caseRegistered = 5;                // choose the pin for the LED
int reportIssueButton = 16;               // choose the input pin (for PIR sensor)
int temp =0;
int address = 0;
byte value;
boolean isSetupDebug=true;

const char *DeviceIdInput;
boolean isResetWifiSettings=true;




void setup()
{
  Serial.begin(9600);
  pinMode(executingSetup, OUTPUT);      // declare LED as output
  pinMode(caseRegistered, OUTPUT);      // declare LED as output
  pinMode(reportIssueButton, INPUT);    // declare sensor as input
  digitalWrite(executingSetup, HIGH);
  if(isSetupDebug==true){Serial.println("[setup] Booting Node MCU ");}
  WiFiManager wifiManager;
  if(isResetWifiSettings){wifiManager.resetSettings();}
  wifiManager.setDebugOutput(false);
  WiFiManagerParameter custom_device_id_hardware("server", "Device Id", DeviceIdInput, 10);
  wifiManager.addParameter(&custom_device_id_hardware);
  wifiManager.autoConnect(AccessPointName, AccessPointPassword);
  DeviceIdInput = custom_device_id_hardware.getValue();
  if(isSetupDebug==true){Serial.printf("[setup] Device Key entered: %s\n", DeviceIdInput);}
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  if(isSetupDebug==true){Serial.println("[setup] Attempting to connect to wifi"); }
  }
  if(isSetupDebug==true){Serial.println("[setup] Connected To Wifi");}
  boolean authenticated=false;
  while(authenticated==false){
  if(isSetupDebug==true){Serial.println("[setup] Attempting to authenticate and connect to server");}
  authenticated=ConnectToCloud();
  delay(1000); 
  }
  digitalWrite(executingSetup, LOW);   
}

boolean ConnectToCloud()
{
  HTTPClient http;
  char payload[400];
  char requestURLSF[150];
  if(isSetupDebug==true){Serial.printf("[ConnectToCloud] DeviceIdInput ::%s\n",DeviceIdInput);}
  if(isSetupDebug==true){Serial.print("[ConnectToCloud] Connecting To Server \n");}
  serverURL=server+DeviceIdInput;
  serverURL.toCharArray(requestURLSF,serverURL.length()+1);
  if(isSetupDebug==true){Serial.printf("[LOC] Request URL  : %s\n", requestURLSF);}
  http.begin(requestURLSF,shaFingerPrint);
  int httpCode = http.GET();
  if (httpCode > 0) {
    if(isSetupDebug==true){Serial.printf("[ConnectToCloud] Connection To Server Successful : %d\n", httpCode);}
   // if (httpCode == HTTP_CODE_OK) {
      String payloadString = http.getString();
      payloadString.toCharArray(payload, payloadString.length() + 1);
      if(isSetupDebug==true){Serial.printf("[ConnectToCloud] PAYLOAD : %s\n", payload);}
    
  // }
  } 
  else {
    if(isSetupDebug==true){Serial.printf("[ConnectToCloud] Failed to connect to server, error: %s\n", http.errorToString(httpCode).c_str());}
    return false;
  }
  http.end();
  if(isSetupDebug==true){Serial.println("[ConnectToCloud] ConnectToCloud Successful");}
  delay(500);
  return true;
  }
  
  
int counter=0; // Counter for loop to send ping at 1 minute interval
void loop(){
     temp = digitalRead(reportIssueButton);
     if(temp == HIGH){
        if(isSetupDebug==true){Serial.printf("[loop] Send Call To Register Case");}
        pingCloud("ERROR");
        digitalWrite(caseRegistered, HIGH);
        delay(3000);
        digitalWrite(caseRegistered, LOW);
       }
       if(counter>=60){
        if(isSetupDebug==true){Serial.printf("[loop] Send PING to salesforce");}
        pingCloud("PING");
        counter=0;
       }
       else{
        counter++;
        delay(100);
       }
       // Code a structure for resetting connection incase of connection loss
}
boolean pingCloud(char *message)
{
  HTTPClient http;
  char payload[400];
  char requestURLSF[150];
  if(isSetupDebug==true){Serial.printf("[pingCloud] DeviceIdInput ::%s\n",DeviceIdInput);}
  serverURL=server+DeviceIdInput+"&process="+message;
  serverURL.toCharArray(requestURLSF,serverURL.length()+1);
  if(isSetupDebug==true){Serial.printf("[pingCloud] Request URL  : %s\n", requestURLSF);}
  http.begin(requestURLSF,shaFingerPrint);
  int httpCode = http.GET();
  if (httpCode > 0) {
    if(isSetupDebug==true){Serial.printf("[pingCloud] Connection To Server Successful : %d\n", httpCode);}
      String payloadString = http.getString();
      payloadString.toCharArray(payload, payloadString.length() + 1);
      if(isSetupDebug==true){Serial.printf("[pingCloud] PAYLOAD : %s\n", payload);}
      serverURL="";
  }
}


  
