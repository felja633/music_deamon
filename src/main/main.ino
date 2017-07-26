#include "Pitches.h"

// Game vars
int level = 1;
int melody_cnt = 0;
int num_melodies = 1;
int user_play = 0;

const int speakerPin = 11;   

//Melody vars
const int theme[] = {11,                                                  // Array for Theme song
  NOTE_E4, 8, NOTE_E4, 8, NOTE_H, 8, NOTE_E4, 8, NOTE_H, 8, NOTE_C4, 8, NOTE_E4, 8, NOTE_H, 8, NOTE_G4, 8, NOTE_H, 3, NOTE_G3, 8};
const int life[] = {6,                                                    // Array for 1-up sound effect
  NOTE_E5, 10, NOTE_G5, 10, NOTE_E6, 10, NOTE_C6, 10, NOTE_D6, 10, NOTE_G6, 10};
const int death[] = {17,                                                  // Array for Death sound effect & song
  NOTE_C4, 32, NOTE_CS4, 32, NOTE_D4, 16, NOTE_H, 4, NOTE_H, 2, NOTE_B3, 8, NOTE_F4, 8, NOTE_H, 8, NOTE_F4, 8, NOTE_F4, 6, 
  NOTE_E4, 6, NOTE_D4, 6, NOTE_C4, 8, NOTE_E3, 8, NOTE_H, 8, NOTE_E3, 8, NOTE_C3, 8};

int** melodies;

//Sensor vars
const int trigPin = 9;
const int echoPin = 10;
const float calib = 1.0;
int current_measurment = 0;
int CHANGE_THRESH = 20;
int FAIL_THRESH = 1000;

int convertToSound(int dist, float scale)
{
  return dist/scale;
}

void playMelody(int* melody)
{
  for (int thisNote = 1; thisNote < (melody[0] * 2 + 1); thisNote = thisNote + 2) { // Run through the notes one at a time
    tone(speakerPin, melody[thisNote], (1000/melody[thisNote + 1]));// Play the single note
    delay((1000/melody[thisNote + 1]) * 1.30);                        // Delay for the specified time
    noTone(speakerPin);                                                 // Silence the note that was playing
  }
}


void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  // put your setup code here, to run once:
  Serial.begin(9600);
  melodies = new int*[num_melodies];
  melodies[0] = theme;
  playMelody(life);
  delay(1000); 
}

void sort(int a[], int size_a)
{
  for(int i = 0; i < (size_a - 1); i++)
  {
    for(int o = 0; o < (size_a - (i + 1)); o++)
    {
      if(a[o] > a[o + 1])
      {
        int t = a[o];
        a[o] = a[o + 1];
        a[o + 1] = t;
      }
    }
  }
}

int sensor_measurements()
{
  int distances[9];
  for(int i = 0; i < 9; i++)
  {
    long duration;
    int distance;
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    //delayMicroseconds(200);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance= duration*0.034/2;
    // Prints the distance on the Serial Monitor
    
    distances[i] = distance;
  }

  sort(distances, 9);
  Serial.print("Distance: ");
  Serial.println(distances[4]);
  return distances[4];
  
}


int userPlayMelody(int* melody)
{
  int success = 1;
  int new_measurment = sensor_measurements();
  while(abs(current_measurment - new_measurment) < CHANGE_THRESH)
  {
    new_measurment = sensor_measurements();
    delay(10);
    Serial.print("Wait for user start ");
  }
  current_measurment = new_measurment;
  unsigned long begin_time;
  unsigned long end_time;
  //Start user melody
  for (int thisNote = 1; thisNote < (melody[0] * 2 + 1); thisNote = thisNote + 2)
  {
    tone(speakerPin, melody[thisNote], (1000/melody[thisNote + 1]));// Play the single note
    
    begin_time = millis();

    new_measurment = sensor_measurements();
    while(abs(new_measurment - current_measurment) < CHANGE_THRESH)
    {
      new_measurment = sensor_measurements();
      delay(10);
      Serial.print("Wait for user to play note: ");
    }
    end_time = millis();
    current_measurment = new_measurment;
    int diff_time = end_time - begin_time;
    if(abs((1000/melody[thisNote + 1]) * 1.30 - diff_time) > FAIL_THRESH)
    {
      //user failed
      success = 0;
      break;
    }                                             // Silence the note that was playing
  }

  return success;
}


void loop() {
  // put your main code here, to run repeatedly:

  int success;
  if(user_play != 1)
  {
    playMelody(melodies[melody_cnt]);
    delay(1000); 
    user_play = 1;
  }
  else
  {
    // bla
    //playMelody(life);
    current_measurment = sensor_measurements();
    success = userPlayMelody(melodies[melody_cnt]);
    if(success == 1)
    {
      melody_cnt = melody_cnt + 1;
      if(melody_cnt >= num_melodies)
      {
        level = level + 1;
        melody_cnt = 0;
      }
      playMelody(life);
    }
    else
    {
      playMelody(death);
      melody_cnt = 0;
    }
    user_play = 0;
  }
}

