/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include <WiFiManager.h> 
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "parjanya"
#define WLAN_PASS       "fireandblood"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "parjanyaroy"
#define AIO_KEY         "a20b8948b5b240edad1b38810b1ef4ef"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
//Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe skypestatusfeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/skypeiot");
/*************************** Wifi Manager ************************************/
WiFiManager wifiManager;

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

const int outputRed = 5;
const int outputGreen = 14;
const int outputYellow = 12;


void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(outputRed, OUTPUT); // For Two Outputs : Relay input 1
  pinMode(outputGreen, OUTPUT); // For Two Outputs : Relay input 2
  pinMode(outputYellow, OUTPUT); // For Two Outputs : Relay input 3
  digitalWrite(outputRed, LOW);// Set outputs to LOW
  digitalWrite(outputGreen, LOW);// Set outputs to LOW
  digitalWrite(outputYellow, LOW);// Set outputs to LOW
  //wifiManager.resetSettings();
  wifiManager.autoConnect("Skype IOT Setup", "skype");

  // Connect to WiFi access point.
  //Serial.println(); Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(WLAN_SSID);

  //WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    digitalWrite(outputRed, HIGH);// Set outputs to LOW
    delay(200);
    digitalWrite(outputGreen, HIGH);// Set outputs to LOW
    delay(200);
    digitalWrite(outputYellow, HIGH);// Set outputs to LOW
    delay(200);
    digitalWrite(outputRed, LOW);// Set outputs to LOW
    digitalWrite(outputGreen, LOW);// Set outputs to LOW
    digitalWrite(outputYellow, LOW);// Set outputs to LOW
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&skypestatusfeed);
}

uint32_t x=0;

void loop() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &skypestatusfeed) {
      Serial.print(F("Got: "));
      Serial.println((char *)skypestatusfeed.lastread);
      char *skypeStatus=(char *)skypestatusfeed.lastread;
      if(strcmp(skypeStatus,"available")==0)
      {
        digitalWrite(outputRed, LOW);// Set outputs to LOW
        digitalWrite(outputGreen, HIGH);// Set outputs to LOW
        digitalWrite(outputYellow, LOW);// Set outputs to LOW
      }
      else if(strcmp(skypeStatus,"busy")==0)
      {
        digitalWrite(outputRed, HIGH);// Set outputs to LOW
        digitalWrite(outputGreen, LOW);// Set outputs to LOW
        digitalWrite(outputYellow, LOW);// Set outputs to LOW
      }
      else if(strcmp(skypeStatus,"idle")==0)
      {
        digitalWrite(outputRed, LOW);// Set outputs to LOW
        digitalWrite(outputGreen, LOW);// Set outputs to LOW
        digitalWrite(outputYellow, HIGH);// Set outputs to LOW
      }
      else if(strcmp(skypeStatus,"offline")==0)
      {
        digitalWrite(outputRed, LOW);// Set outputs to LOW
        digitalWrite(outputGreen, LOW);// Set outputs to LOW
        digitalWrite(outputYellow, LOW);// Set outputs to LOW
      } 
      
    }
  }
}

void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
