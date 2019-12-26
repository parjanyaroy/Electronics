#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <string.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE); 
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display


// -------------------------------- AUTO CONNECT MANAGER DATA --------------------------------------------//
char AccessPointName[] = "Home Automation Setup";
char AccessPointPassword[] = "Home@1234";
int logdelay = 3000 ; // Delay In LCD Display
// -------------------------------- AUTO CONNECT MANAGER DATA --------------------------------------------//

// -------------------------------- AUTO CONNECT DEVICE ID DATA--------------------------------------------//
String server = "https://homeautomationalpha-developer-edition.ap4.force.com/services/apexrest/iotservice?deviceid="; //DIRECT SFDC CONNECT &process=PING&deviceparam=29
String serverURL =""; 
char shaFingerPrint[]="3A:47:D5:0C:34:B5:A9:E9:38:BA:5B:FB:46:4C:81:57:AD:D5:A5:59";
String deviceIdString="sku-000002";
// -------------------------------- AUTO CONNECT DEVICE ID --------------------------------------------//


WiFiServer DeviceServer(1025);


int executingSetup= 13;
int caseRegistered = 12;                // choose the pin for the LED
int reportIssueButton = 14;               // choose the input pin (for PIR sensor)
int temp =0;
int address = 0;
byte value;
boolean isSetupDebug=true;

const char *DeviceIdInput;
boolean isResetWifiSettings=false;




void setup()
{
  Serial.begin(9600);
  lcd.begin();
  dht.begin();
  lcd.backlight();
  pinMode(executingSetup, OUTPUT);      // declare LED as output
  pinMode(caseRegistered, OUTPUT);      // declare LED as output
  pinMode(reportIssueButton, INPUT);    // declare sensor as input
  digitalWrite(executingSetup, HIGH);
  if(isSetupDebug==true){Serial.println("[setup] Booting Node MCU ");
  lcd_print("-$upp0rt__$qu@d-",logdelay);}
  WiFiManager wifiManager;
  wifiManager.setDebugOutput(true);
  wifiManager.autoConnect(AccessPointName, AccessPointPassword);
  //if(isSetupDebug==true){Serial.printf("[setup] Device Key entered: %s\n", DeviceIdInput);}
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  if(isSetupDebug==true){Serial.println("[setup] Attempting to connect to wifi"); 
  lcd_print("Connecting WIFI",logdelay);}
  }
  if(isSetupDebug==true){Serial.println("[setup] Connected To Wifi");
  lcd_print("WIFI Connected",logdelay);}
  boolean authenticated=false;
  while(authenticated==false){
  if(isSetupDebug==true){Serial.println("[setup] Attempting to authenticate and connect to server");
  lcd_print("Authenticating ",logdelay);}
  authenticated=ConnectToCloud();
  delay(5000); 
  }
  lcd_print("Setup Complete",logdelay);
  digitalWrite(executingSetup, LOW);
}

boolean ConnectToCloud()
{
  lcd_print("Connecting SF ",logdelay);
  HTTPClient http;
  char payload[400];
  char requestURLSF[150];
  if(isSetupDebug==true){Serial.println("[ConnectToCloud] DeviceIdInput ::"+deviceIdString);}
  if(isSetupDebug==true){Serial.print("\n[ConnectToCloud] Connecting To Server \n");}
  serverURL=server+deviceIdString;
  serverURL.toCharArray(requestURLSF,serverURL.length()+1);
  if(isSetupDebug==true){Serial.printf("[LOC] Request URL  : %s\n", requestURLSF);}
  if(isSetupDebug==true){Serial.printf("[LOC] Fingerprint  : %s\n", shaFingerPrint);}
  http.begin(requestURLSF,shaFingerPrint);
  int httpCode = http.GET();
  if (httpCode > 0) {
    if(isSetupDebug==true){Serial.printf("[ConnectToCloud] Connection To Server Successful : %d\n", httpCode);}
    if (httpCode == HTTP_CODE_OK) {
      String payloadString = http.getString();
      payloadString.toCharArray(payload, payloadString.length() + 1);
      if(isSetupDebug==true){Serial.printf("[ConnectToCloud] PAYLOAD : %s\n", payload);
      lcd_print("Connected To SF",logdelay);}
    
   }
  } 
  else {
    if(isSetupDebug==true){Serial.printf("[ConnectToCloud] Failed to connect to server, error: %s\n", http.errorToString(httpCode).c_str());
    lcd_print("Connection Failed",logdelay);}
    return false;
  }
  http.end();
  if(isSetupDebug==true){Serial.println("[ConnectToCloud] ConnectToCloud Successful");}
  delay(500);
  return true;
  }
  
  
      int counter=0; // Counter for loop to send ping at 1 minute interval
      void loop(){
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      
     temp = digitalRead(reportIssueButton);
     if(temp == HIGH){
        if(isSetupDebug==true){Serial.printf("[loop] Send Call To Register Case");}
        digitalWrite(caseRegistered, HIGH);
        pingCloud("ERROR",t,h);
        digitalWrite(caseRegistered, LOW);
       }
       if(counter>=60){
        if(isSetupDebug==true){Serial.printf("[loop] Send PING to salesforce");}
        pingCloud("PING",t,h);
        counter=0;
       }
       else{
         if(isSetupDebug==true){Serial.printf("\n[loop] Incrementing Counter");}
          lcd_print_float(t,h);
          
        counter++;
        delay(100);        
       }
       // Code a structure for resetting connection incase of connection loss
}
boolean pingCloud(char *message,float temperature,float humidity)
{
  HTTPClient http;
  char payload[400];
  char requestURLSF[200];
  char charvalueT[5];
  sprintf(charvalueT, "%.1f", temperature);
  char charvalueH[5];
  sprintf(charvalueH, "%.0f", humidity);
  if(isSetupDebug==true){Serial.println("[pingCloud] DeviceIdInput ::"+deviceIdString);}
   if(isSetupDebug==true){Serial.printf("[pingCloud] charvalueT ::%s",charvalueT);}
    if(isSetupDebug==true){Serial.printf("[pingCloud] charvalueH ::%s",charvalueH);}
  serverURL=server+deviceIdString+"&process="+message+"&deviceparam1="+charvalueT+"&deviceparam2="+charvalueH;
  serverURL.toCharArray(requestURLSF,serverURL.length()+1);
  if(isSetupDebug==true){Serial.printf("\n[pingCloud] Request URL  : %s\n", requestURLSF);}
  http.begin(requestURLSF,shaFingerPrint);
  int httpCode = http.GET();
  if (httpCode > 0) {
    if(isSetupDebug==true){Serial.printf("[pingCloud] Connection To Server Successful : %d\n", httpCode);}
      String payloadString = http.getString();
      payloadString.toCharArray(payload, payloadString.length() + 1);
      if(isSetupDebug==true){Serial.printf("[pingCloud] PAYLOAD : %s\n", payload);}
      lcd_print_and_clear(payload,logdelay);
      serverURL="";
  }
}

void lcd_print_and_clear(String message,int delayParam)
{
  lcd.clear();
  lcd.print(message);
  delay(delayParam);
  lcd.clear();
}

void lcd_print(String message,int delayParam)
{
  lcd.clear();
  lcd.print(message);
  delay(delayParam);
}

void lcd_print_float(float tValue,float hValue)
{
  char charvalueT[5];
  sprintf(charvalueT, "%.1f", tValue);
  char charvalueH[5];
  sprintf(charvalueH, "%.0f", hValue);
  lcd.clear();
  lcd.print("T:");
  lcd.print(charvalueT);
  lcd.print("C,H:");
  lcd.print(charvalueH);
  lcd.print("%");
}


  
