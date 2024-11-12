#include <NewPing.h> 
#define SONAR_NUM 5      // Number of sensors. 

#define MAX_DISTANCE 30  // Maximum distance (in centimeters) we want to ping for. 
// Define the pins for each sensor's trigger and echo pins. 
// The format is {TRIGGER_PIN, ECHO_PIN} 
const uint8_t sonar_pins[SONAR_NUM][2] = { 
{ 10, 9 },   // Sensor 1: TRIGGER_PIN = 10, ECHO_PIN = 9 
{ 12, 11 },  // Sensor 2: TRIGGER_PIN = 11, ECHO_PIN = 8 
{ 3, 4 },    
// Sensor 3: TRIGGER_PIN = 12, ECHO_PIN = 7 

{ 5, 6 },    
// Sensor 4: TRIGGER_PIN = 13, ECHO_PIN = 6 
{ 7, 8 }     
digital) 
}; 
// Sensor 5: TRIGGER_PIN = A0, ECHO_PIN = A1 (using analog pins as 
NewPing sonars[SONAR_NUM] = {                       
// Sensor object array. 
NewPing(sonar_pins[0][0], sonar_pins[0][1], 15),  // NewPing setup for each sensor. 
NewPing(sonar_pins[1][0], sonar_pins[1][1], MAX_DISTANCE - 10), 
NewPing(sonar_pins[2][0], sonar_pins[2][1], MAX_DISTANCE + 10), 
NewPing(sonar_pins[3][0], sonar_pins[3][1], MAX_DISTANCE - 10), 
NewPing(sonar_pins[4][0], sonar_pins[4][1], 15) 
}; 
#include <filters.h> 
const float cutoff_freq = 16.0;      
//Cutoff frequency in Hz 
const float sampling_time = 0.01;    //Sampling time in seconds. 
IIR::ORDER order = IIR::ORDER::OD1;  // Order (OD1 to OD4) 
// Low-pass filter 
Filter f(cutoff_freq, sampling_time, order); 
Filter fDistances[SONAR_NUM] = { 
Filter(cutoff_freq, sampling_time, order), 
Filter(cutoff_freq, sampling_time, order), 
Filter(cutoff_freq, sampling_time, order), 
Filter(cutoff_freq, sampling_time, order), 
Filter(cutoff_freq, sampling_time, order) 
}; 

float distances[SONAR_NUM] = {}; 

 
long long timerClock = 0; 
const int interval = 100; 
 
void setup() { 
  Serial.begin(38400); 
  pinMode(A0, INPUT); 
  // Enable pull-ups if necessary 
  digitalWrite(A0, HIGH); 
} 
 
void loop() { 
  for (int i = 0; i < SONAR_NUM; i++) { 
    distances[i] = fDistances[i].filterIn(sonars[i].ping_cm()); 
  } 
  delay(10); 
 
  if (millis() - timerClock > interval) 
    transferData(); 
} 
 
void transferData() { 
  timerClock = millis(); 
  for (int i = 0; i < SONAR_NUM; i++) { 
    Serial.print(int(distances[i])); 
    Serial.print(", "); 
  } 
  Serial.println(); 
}
