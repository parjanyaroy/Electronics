#include "pitches.h"  //add note library

//notes in the melody
int melody[]={NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};

//digital pin 12 has a button attached to it. Give it an name
int buttonPinBuzzer= 12;

//note durations. 4=quarter note / 8=eighth note
int noteDurations[]={4, 8, 8, 4, 4, 4, 4, 4};

//set pin numbers
const int ledPin = 2;         //const won't change
const int buttonPin = 4;
const int DimmerLEDPin=9;

//variables will change
int buttonState = 0;          //variables for reading the pushbutton status

void setup() {

  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);    //initialize the LED pin as an output
  pinMode(DimmerLEDPin, OUTPUT);    //initialize the LED pin as an output
  pinMode(buttonPin, INPUT);  //initialize the pushbutton pin as an output
  pinMode(buttonPinBuzzer, INPUT);  //initialize the pushbutton pin as an output
}

void loop() {

  buttonState = digitalRead(buttonPin); //read the state of the pushbutton value

  if (buttonState == HIGH) {            //check if the pushbutton is pressed
    //if it is, the buttonState is HIGH
    digitalWrite(ledPin, HIGH);         //turn LED on    
    Serial.println("LED ON +++++++"); 
  }
  else {

    digitalWrite(ledPin, LOW);          // turn LED off
    //Serial.println("LED OFF -------");
  }

  int sensorValue = analogRead(A0); //read the input on analog pin 0
  double writeValue =(sensorValue*255)/1023;
  Serial.println(writeValue); //print out the value you read
  analogWrite(DimmerLEDPin,writeValue);

  delay(10); //delay in between reads for stability


  int buttonStatebuzzer = digitalRead(buttonPinBuzzer);

  //if the button is pressed
  if (buttonStatebuzzer == 1){

    //iterate over the notes of the melody
    for (int thisNote=0; thisNote <8; thisNote++){

      //to calculate the note duration, take one second. Divided by the note type
      int noteDuration = 1000 / noteDurations [thisNote];
      tone(8, melody [thisNote], noteDuration);

      //to distinguish the notes, set a minimum time between them
      //the note's duration +30% seems to work well
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);

      //stop the tone playing
      noTone(8);
    }
  }

}
