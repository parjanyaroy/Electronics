#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE); 
void setup(void)
{ 
  dht.begin();
  Serial.begin(9600);
  Serial.println("Humidity and temperature\n\n");
  lcd.init();   // initializing the LCD
 // lcd.backlight(); // Enable or Turn On the backlight 
  lcd.setCursor(0, 1);
  lcd.print("14CORE | 16x2 LCD TEST"); // Start Print text to Line 1
  lcd.setCursor(0, 2);      
  lcd.print("-----------------------"); // Start Print Test to Line 2
  delay(700);

}
void loop() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();      
    //lcd.print("Hello, NodeMCU");   
    Serial.print("Current humidity = ");
    Serial.print(h);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(t); 
    Serial.println("C  ");
  delay(800);
}
