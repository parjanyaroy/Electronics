#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>



const char* ssid     = "dadsadas";                 // SSID of local network
const char* password = "dsadsadas";                    // Password on network
String APIKEY = "442019af80dcc4a812bbfd68f4c1af1d";                                 
String CityID = "1269843";                                 //Your City ID


WiFiClient client;
char servername[]="api.openweathermap.org";              // remote server we will connect to
String result;

int  counter = 60;                                      

String weatherDescription ="";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;

void setup() {
  Serial.begin(115200);
  int cursorPosition=0;
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  
             while (WiFi.status() != WL_CONNECTED) 
            {
            delay(500);
            cursorPosition++;
            }
  Serial.println("Connected");
  delay(1000);
}

void loop() {
  Serial.println("===========================");
  Serial.println();
    if(counter == 60)                                 //Get new data every 10 minutes
    {
      counter = 0;
      delay(1000);
      getWeatherData();
    }else
    {
      counter++;
      Serial.println("weatherLocation "+weatherLocation);
    Serial.println("weatherDescription "+weatherDescription);
      delay(5000);
      Serial.println("Temperature ");
      Serial.print(Temperature);
    Serial.println("Humidity ");
    Serial.print(Humidity);
    Serial.println("Pressure ");
    Serial.print(Pressure);
    delay(5000);
    }
}

void getWeatherData()                                //client function to send/receive GET request data.
{
  if (client.connect(servername, 80))   
          {                                         //starts client connection, checks for connection
          client.println("GET /data/2.5/weather?id="+CityID+"&units=metric&APPID="+APIKEY);
          client.println("Host: api.openweathermap.org");
          client.println("User-Agent: ArduinoWiFi/1.1");
          client.println("Connection: close");
          client.println();
          } 
  else {
         Serial.println("connection failed");        //error message if no client connect
          Serial.println();
       }

  while(client.connected() && !client.available()) 
  delay(1);                                          //waits for data
  while (client.connected() || client.available())    
       {                                             //connected or data available
         char c = client.read();                     //gets byte from ethernet buffer
         result = result+c;
       }

client.stop();                                      //stop client
result.replace('[', ' ');
result.replace(']', ' ');
Serial.println(result);
char jsonArray [result.length()+1];
result.toCharArray(jsonArray,sizeof(jsonArray));
jsonArray[result.length() + 1] = '\0';
StaticJsonBuffer<1024> json_buf;
JsonObject &root = json_buf.parseObject(jsonArray);

if (!root.success())
  {
    Serial.println("parseObject() failed");
  }

String location = root["name"];
String country = root["sys"]["country"];
float temperature = root["main"]["temp"];
float humidity = root["main"]["humidity"];
String weather = root["weather"]["main"];
String description = root["weather"]["description"];
float pressure = root["main"]["pressure"];
weatherDescription = description;
weatherLocation = location;
Country = country;
Temperature = temperature;
Humidity = humidity;
Pressure = pressure;

}

