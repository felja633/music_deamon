//Photsensor
int pin = 1;    

void setup() {
pinMode(pin, OUTPUT); // Sets the trigPin as an Output
Serial.begin(9600); // Starts the serial communication
}

void loop()
{
  tone(pin, 500);
  delayMicroseconds(2);
}
