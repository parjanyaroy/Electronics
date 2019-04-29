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


struct DeviceData {
  boolean isResetWifiSettings=true;
  const char *deviceId;
};

void setup()
{
  Serial.begin(9600);
  
  struct DeviceData fetchedData=getDeviceData();
  getDeviceData();
  pinMode(executingSetup, OUTPUT);      // declare LED as output
  pinMode(caseRegistered, OUTPUT);      // declare LED as output
  pinMode(reportIssueButton, INPUT);     // declare sensor as input
  digitalWrite(executingSetup, HIGH);
  Serial.println("");
  if(isSetupDebug==true){Serial.println("[LOC] Booting Node MCU ");}
  
  WiFiManager wifiManager;
  if(isResetWifiSettings){wifiManager.resetSettings();}
  
 // wifiManager.setDebugOutput(isSetupDebug);
  WiFiManagerParameter custom_device_id_hardware("server", "Device Id", DeviceIdInput, 10);
  wifiManager.addParameter(&custom_device_id_hardware);
  wifiManager.autoConnect(AccessPointName, AccessPointPassword);
  DeviceIdInput = custom_device_id_hardware.getValue();
  if(isSetupDebug==true){Serial.printf("[LOC] Device Key entered: %s\n", DeviceIdInput);}
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  if(isSetupDebug==true){Serial.println("[LOC] Attempting to connect to wifi"); }
  }
  if(isSetupDebug==true){Serial.println("[LOC] Connected To Wifi");}
  boolean authenticated=false;
  while(authenticated==false)
  {
  if(isSetupDebug==true){Serial.println("[LOC] Attempting to authenticate and connect to server");}
  authenticated=ConnectToCloud();
  delay(1000); 
  }

  DeviceData customVar ;
  customVar.isResetWifiSettings=false;
  customVar.deviceId=DeviceIdInput;
  setDeviceData(customVar);
  getDeviceData();
  //digitalWrite(output_Boot, LOW);
  digitalWrite(executingSetup, LOW);
   
}

void setDeviceData(struct DeviceData record)
{
  Serial.printf("\nEEPROM:: PUT : isReset: %d deviceId: %s",record.isResetWifiSettings,record.deviceId);
  EEPROM.begin(512);
  EEPROM.put(0, record);
  EEPROM.commit();
}

struct DeviceData getDeviceData()
{
  struct DeviceData fetchedRecord;
  EEPROM.get( 0, fetchedRecord );
  Serial.printf("\nEEPROM:: GET : isReset: %d deviceId: %s",fetchedRecord.isResetWifiSettings,fetchedRecord.deviceId);
  return fetchedRecord;
}


boolean ConnectToCloud()
{
  HTTPClient http;
  char payload[400];
  char requestURLSF[150];
  
  // Logic to generate the master token
  String dataToken = DeviceIdInput;// First 20 characters will be the device Id , remaining will be the timestamp
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
  if(isSetupDebug==true){Serial.print("[LOC] Datatoken \n"+dataToken);}
  if(isSetupDebug==true){Serial.print("[LOC] Connecting To Server \n");}
  server=server+dataToken;
  server.toCharArray(requestURLSF,server.length()+1);
  if(isSetupDebug==true){Serial.printf("[LOC] Request URL  : %s\n", requestURLSF);}
  http.begin(requestURLSF,shaFingerPrint);
  int httpCode = http.GET();
  if (httpCode > 0) {
    if(isSetupDebug==true){Serial.printf("[LOC] Connection To Server Successful : %d\n", httpCode);}
   // if (httpCode == HTTP_CODE_OK) {
      String payloadString = http.getString();
      payloadString.toCharArray(payload, payloadString.length() + 1);
      if(isSetupDebug==true){Serial.printf("[LOC] PAYLOAD : %s\n", payload);}
    
  // }
  } 
  else {
    if(isSetupDebug==true){Serial.printf("[LOC] Failed to connect to server, error: %s\n", http.errorToString(httpCode).c_str());}
    return false;
  }
  http.end();
  if(isSetupDebug==true){Serial.println("[LOC] ConnectToCloud Successful");}
  delay(500);
  return true;
  }
  int counter=0;
void loop(){
     temp = digitalRead(reportIssueButton);
     if (temp == HIGH) {
        Serial.println("Send Call To Register Case");
        digitalWrite(caseRegistered, HIGH);
        delay(3000);
        digitalWrite(caseRegistered, LOW);
       }

       if(counter>=600)
       {
        Serial.println("Send PING to salesforce");
        counter=0;
       }
       else
       {
        counter++;
        delay(100);
       }
     
  //Serial.println("Loop starts");
    // if button pressed 
  //{
  //  -> send request to salesforce for registering case
  // -> read response on successful case registration
  // switch on the led 
  //-> set case registered url
  // delay for 2 seconds to make sure button is not read continuously
  // switch off the led
  //}
  // if counter is 600
  // { send status ping to salesforce , set counter to 0}
  // else
  // { increment counter , delay 100 milisecond second}
}
  
