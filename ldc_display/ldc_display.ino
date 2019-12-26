#include <LiquidCrystal_I2C.h>

#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
char Name1[] = "Home Automation Setup";
void setup() {

Serial.begin(115200);

//Use predefined PINS consts

Wire.begin(D2, D1);

lcd.begin();

lcd.home();

lcd.print("Hello, NodeMCU");
delay(5000);
lcd.clear();
lcd.write(Name1);
/*lcd.home();
  lcd.print("Hello, NodeMCU");
  lcd.clear();
  delay(5000);
  lcd.home();
  lcd.print("Hello, Suchandra");
  lcd.clear();
  delay(5000);
  lcd.home();
  lcd.print("Hello, Roy");
  lcd.clear();
  delay(5000);
  lcd.home();
  lcd.print("Hello, Long Text");
  lcd.clear();
  delay(1000);
  lcd.print("Hello, Last Text");
  lcd.clear();
  delay(1000);*/
}

void loop() { // do nothing here
  
  }
