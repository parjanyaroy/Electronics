#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

boolean isSetupDebug=true;
boolean isResetWifiSettings=false;

// -------------------------------- TIME ZONE AUTHENTICATION DATA --------------------------------------------//
String dateApiName = "http://api.timezonedb.com/v2/get-time-zone?key=";
String apiKey = "RKEX6GHOJMTH";
String zonePefix="&format=json&by=zone&zone=";
String zone = "Asia/Kolkata";
StaticJsonBuffer<1024> json_buf;
// -------------------------------- TIME ZONE AUTHENTICATION DATA --------------------------------------------//


// -------------------------------- SALESFORCE SERVER DATA --------------------------------------------//
//String servername = "homeautomationalpha-developer-edition.ap4.force.com";            // remote server we will connect to
// -------------------------------- SALESFORCE SERVER DATA --------------------------------------------//

// -------------------------------- AUTO CONNECT MANAGER DATA --------------------------------------------//
char AccessPointName[] = "Home Automation Setup";
char AccessPointPassword[] = "Home@1234";
// -------------------------------- AUTO CONNECT MANAGER DATA --------------------------------------------//


String result;

// -------------------------------- AUTO CONNECT DEVICE ID DATA--------------------------------------------//
String server = "https://homeautomationalpha-developer-edition.ap5.force.com/services/apexrest/iotservice?DeviceId=";
String DeviceId="82542347168123446895"; // Should be restricted to 20 characters and Numeric
char shaFingerPrint[]="D1 A3 3C D7 D5 87 0A 10 81 22 BF 44 12 B8 C8 7B 1A D2 DC 94";
// -------------------------------- AUTO CONNECT DEVICE ID --------------------------------------------//


WiFiServer DeviceServer(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

// Assign output variables to GPIO pins
const int output5 = 5;
const int output4 = 4;


void setup()
{
  if(isSetupDebug==true){Serial.println("[LOC] Booting Node MCU ");}
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
   pinMode(output5, OUTPUT); // For Two Outputs : Relay input 1
  pinMode(output4, OUTPUT); // For Two Outputs : Relay input 2
  // Set outputs to LOW
  digitalWrite(output5, HIGH);
  digitalWrite(output4, HIGH);
  DeviceServer.begin();
}

boolean ConnectToCloud()
{
  HTTPClient http;
  char payload[400];
  char requestURL[100];
  char requestURLSF[150];
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
  String dataToken="";
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
  String dataToken = DeviceId+timestamp;// First 20 characters will be the device Id , remaining will be the timestamp
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
  //http.begin("https://homeautomationalpha-developer-edition.ap5.force.com/services/apexrest/iotservice?DeviceId=1231231231242312312", "D1 A3 3C D7 D5 87 0A 10 81 22 BF 44 12 B8 C8 7B 1A D2 DC 94");
  if(isSetupDebug==true){Serial.printf("[LOC] Request URL  : %s\n", requestURLSF);}
  http.begin(requestURLSF,shaFingerPrint);
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
}

void loop() {

  
 WiFiClient client = DeviceServer.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
              digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
              digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              digitalWrite(output4, LOW);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              digitalWrite(output4, HIGH);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>Cloud Socket (Alpha Stage)</h1>");

            // Display current state, and ON/OFF buttons for GPIO 5
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button
            if (output5State == "off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for GPIO 4
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button
            if (output4State == "off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}


