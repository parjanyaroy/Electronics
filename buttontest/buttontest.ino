#include <ESP8266WiFi.h>
const int BUTTON = 14;
const int LED = 0;
int BUTTONstate = 0;

void setup(){ 
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode (BUTTON, INPUT);
 }
 
void loop() {
  BUTTONstate = digitalRead(BUTTON); 
  Serial.print(BUTTONstate);
  if( BUTTONstate == HIGH)
  {
    Serial.print("HIGH!!!");
  }
  delay(1000);
}
