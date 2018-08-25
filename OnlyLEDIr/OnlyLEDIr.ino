#include <IRremote.h>

int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

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
  pinMode(blue_LED, OUTPUT);
  pinMode(red_LED, OUTPUT);
  pinMode(green_LED, OUTPUT);
  pinMode(black_LED, OUTPUT);
  
  irrecv.enableIRIn();
}
void loop() {

  if (irrecv.decode(&results))
  {
    
    if (results.value == BUTTON_1)
    {
      digitalWrite(blue_LED, HIGH);
      delay(5000);
      digitalWrite(blue_LED, LOW);
    }
    if (results.value == BUTTON_2)
    {
      digitalWrite(red_LED, HIGH);
      delay(5000);
      digitalWrite(red_LED, LOW);
    }
    if (results.value == BUTTON_3)
    {
      digitalWrite(green_LED, HIGH);
      delay(5000);
      digitalWrite(green_LED, LOW);
      
    }
  if (results.value == BUTTON_4)
    {
      digitalWrite(black_LED, HIGH);
      delay(5000);
      digitalWrite(black_LED, LOW);
    }
    irrecv.resume();
  }
}
