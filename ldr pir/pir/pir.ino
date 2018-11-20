/*
 * PIR sensor tester
 */
 
int ledPin = 13;                // choose the pin for the LED
int inputPin = 2;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
int counter=0;
int sensorPin = A0; // select the input pin for LDR

int sensorValue = 0; // variable to store the value coming from the sensor

void setup() {
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input
  Serial.println("Hi!");
  Serial.begin(9600);
  digitalWrite(ledPin, HIGH); // turn LED OFF
}
 
void loop(){
  sensorValue = analogRead(sensorPin); // read the value from the sensor
  Serial.print("sensorValue ");
  Serial.print(sensorValue);
  if(sensorValue<250)
  {
    Serial.println("1.  Room Light Off!!");
    delay(1000);
    val = digitalRead(inputPin);  // read input value
    if(val==HIGH)
    {
      Serial.println("2.  Movement Found Turning On Lamp");
      digitalWrite(ledPin, LOW); // turn LED ON
        while(counter<600 && analogRead(sensorPin)<250)
        {
        val = digitalRead(inputPin);
        if(val==LOW)
        {
          counter++;
          //Serial.println("3.  No Motion.Incrementing Counter To ");
          Serial.println(counter);
        }
        else
        {
          counter=0;
          Serial.println("3.  Motion Detected.Reset Counter");
        }
        delay(1000); // Wait for a second
        }
        counter=0;
        sensorValue = 0;
        val = 0;
    }
    else
    {
      delay(1000);
      digitalWrite(ledPin, HIGH);
    }
  }
  else
  {
    Serial.println("1.  Room Light On!!");
    digitalWrite(ledPin, HIGH); // turn LED OFF
    delay(5000);
  }
  
}
