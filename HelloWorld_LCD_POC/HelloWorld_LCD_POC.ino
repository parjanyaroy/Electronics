#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE); 
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
	lcd.begin();
  dht.begin();
  lcd.backlight();
  Serial.begin(9600);
}

void loop()
{	
  lcd_print("Hello, Roy!",1000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  lcd_print_float("Temp(C):",t,3000);
  lcd_print_float("Humi(%):",h,3000);  
}

void lcd_print(String message,int delayParam)
{
  lcd.clear();
  lcd.print(message);
  delay(delayParam);
}

void lcd_print_float(String message,float value,int delayParam)
{
  char charvalue[5];
  sprintf(charvalue, "%.2f", value);
  lcd.clear();
  lcd.print(message);
  lcd.print(charvalue);
  delay(delayParam);
}
