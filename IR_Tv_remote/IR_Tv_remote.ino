#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

 int buzz=13;                   //piezo buzzer to digital pin 13  
 int senRead=0;                 //Readings from sensor to analog pin 0  
 int limit=850;                 //Threshold range of an obstacle  

int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

// TV REMOTE RELATED CODE ------------------------------------------------------

#define BUTTON_1 0xEF649B
#define BUTTON_2 0xEFE41B
#define BUTTON_3 0xEF14EB
#define BUTTON_4 0xEF946B
#define BUTTON_OK 0x35

int blue_LED = 2;
int red_LED = 3;
int green_LED = 4;
int black_LED = 5;
 
 void setup()    
 {  
  pinMode(buzz,OUTPUT);  
  digitalWrite(buzz,LOW);      //set the buzzer in off mode (initial condition)  
  Serial.begin(9600);          //setting serial monitor at a default baund rate of 9600 
// TV REMOTE RELATED CODE ------------------------------------------------------
   pinMode(blue_LED, OUTPUT);
  pinMode(red_LED, OUTPUT);
  pinMode(green_LED, OUTPUT);
  pinMode(black_LED, OUTPUT);
  
  irrecv.enableIRIn();
 }  
 void loop()  
 { 
  int oldVal=0; 
  int val=analogRead(senRead);  //variable to store values from the photodiode  
  if(val-oldVal>10)
  {
  Serial.println(val);          // prints the values from the sensor in serial monitor  
  oldVal=val;
  }
  if(val <= limit)              //If obstacle is nearer than the Threshold range  
  {  
   digitalWrite(buzz,HIGH);     // Buzzer will be in ON state  
   delay(20);  
  }  
  else if(val > limit)          //If obstacle is not in Threshold range  
  {  
   digitalWrite(buzz,LOW);      //Buzzer will be in OFF state  
   delay(20);  
  }
// ------------------------------------------------ TV Remote Code -------------------------------------
if (irrecv.decode(&results))
  {
    
    if (results.value == BUTTON_1)
    {
      digitalWrite(blue_LED, HIGH);
      delay(2000);
      digitalWrite(blue_LED, LOW);
    }
    if (results.value == BUTTON_2)
    {
      digitalWrite(red_LED, HIGH);
      delay(2000);
      digitalWrite(red_LED, LOW);
    }
    if (results.value == BUTTON_3)
    {
      digitalWrite(green_LED, HIGH);
      delay(2000);
      digitalWrite(green_LED, LOW);
      
    }
  if (results.value == BUTTON_4)
    {
      digitalWrite(black_LED, HIGH);
      delay(2000);
      digitalWrite(black_LED, LOW);
    }
    irrecv.resume();
  }

    
 }  
