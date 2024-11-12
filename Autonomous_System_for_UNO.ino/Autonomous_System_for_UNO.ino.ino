#include <Encoder_Motor.h> 
#include <SoftwareSerial.h> 
 
// Initializing motor objects 
EncoderMotor motorL(4, 2, 5), motorR(7, 3, 6); 
 
// Initializing software serial 
SoftwareSerial softSerial(10, 11);  // Tx, Rx 
 
// Char array to store the incoming data 
const int maxDataLength = 100; 
char receivedData[maxDataLength]; 
int dataIndex = 0; 
int distancArray[5];  // Assuming we know there are 5 integers in the input 
 
 
void refresh() { 
  // Receive and store data from sonalModule if available 
  while (softSerial.available() > 0) { 
    char receivedChar = softSerial.read(); 
 
    // Check for the newline character 
    if (receivedChar == '\n' || dataIndex >= maxDataLength - 1) { 
      // Null-terminate the received string 
      receivedData[dataIndex] = '\0'; 
 
      // Reset dataIndex for the next string 
      dataIndex = 0; 
    } else { 

 
      // Store the received character in the array 
      receivedData[dataIndex] = receivedChar; 
      dataIndex++; 
    } 
 
    if (softSerial.available() == 0) { 
      // char inputCharArray[] = "0, 11, 2, 45, 10,"; 
      int charArrayLength = sizeof(receivedData) - 1;  // -1 to ignore the null terminator 
 
      charArrayToIntArray(receivedData, charArrayLength, distancArray); 
    } 
  } 
} 
 
// Function to convert the char array to an int array 
void charArrayToIntArray(char* charArray, int charArrayLength, int* intArray) { 
  int currentNumber = 0;  // To accumulate the current number being processed 
  int intArrayIndex = 0;  // Index for the intArray where the next number will be stored 
 
  for (int i = 0; i < charArrayLength; ++i) { 
    if (charArray[i] >= '0' && charArray[i] <= '9') { 
      // If the current character is a digit, accumulate it into currentNumber 
      currentNumber = currentNumber * 10 + (charArray[i] - '0'); 
    } else if (charArray[i] == ',' || i == charArrayLength - 1) { 
      // If the current character is a comma or we're at the end of the array, store the number 
      intArray[intArrayIndex++] = currentNumber; 
      currentNumber = 0;  // Reset currentNumber for the next integer 
    } 
    // Ignore any other characters (like spaces) 
  } 

 
} 
 
// Software interupt function 
ISR(TIMER1_COMPA_vect) { 
  refresh();  // Call refresh directly 
} 
 
void setup() { 
  // Begin hardware serial 
  Serial.begin(38400); 
  // Begin software serial 
  softSerial.begin(38400); 
 
  // Begin motor setup 
  motorR.begin(); 
  motorL.begin(); 
  // Enable motors 
  motorR.attach(); 
  motorL.attach(); 
 
  // Setup software interupt 
  cli();  // Disable global interrupts 
 
  // Configure Timer1 
  TCCR1A = 0;  // Clear Timer1 Control Register A 
  TCCR1B = 0;  // Clear Timer1 Control Register B 
  TCNT1 = 0;   // Initialize counter value to 0 
  // Set CTC mode (Clear Timer on Compare Match) 
  TCCR1B |= (1 << WGM12); 
  // Set prescaler to 64 

 
  TCCR1B |= (1 << CS11) | (1 << CS10); 
  // Calculate the compare match value for a 50ms interval 
  OCR1A = 12499;  // For 50ms at 16MHz clock with prescaler of 64 
  // Enable Timer1 compare interrupt 
  TIMSK1 |= (1 << OCIE1A); 
 
  sei();  // Enable global interrupts 
} 
 
int lastdis = 8; 
 
void loop() { 
 
  // if (lastdis != 0 && distancArray[0] == 0) { 
  //   lastdis = distancArray[0]; 
  //   return; 
  // } 
  // lastdis = distancArray[0]; 
 
 
 
  int ccd = 4;   // Corner collide distance 
  int fcd = 30;  // Front colide distance 
 
  if (distancArray[2] == 0) {  // There is nothing in front 
 
    if ((distancArray[1] > 0 && distancArray[1] < ccd) && (distancArray[3] > 0 && distancArray[3] < ccd)) {  // Both corner sides are blocked 
      motorR.drive(80); 
      motorL.drive(80); 
    } else if (distancArray[1] > 0 && distancArray[1] < ccd) {  // Righ-front is about to colide 

 
      motorR.drive(80); 
      motorL.drive(-80); 
    } else if (distancArray[3] > 0 && distancArray[3] < ccd) {  // Left-front is about to colide 
      motorR.drive(-80); 
      motorL.drive(80); 
    } 
    // So far, there is nothing infront and both the corners are free. 
 
 
 
    else if (distancArray[0] == 0 && distancArray[4] == 0) {  // Both right and left sensors are out of range 
      // Just simply go forward 
      motorR.drive(80); 
      motorL.drive(80); 
 
    } else if (distancArray[0] != 0 && distancArray[4] != 0) {  // Both right and left sensors are in range 
      int rDistance = distancArray[0]; 
      int lDistance = distancArray[4]; 
 
      if (rDistance > lDistance) { 
        motorR.drive(70); 
        motorL.drive(100); 
      } else if (rDistance < lDistance) { 
        motorR.drive(100); 
        motorL.drive(70); 
      } else { 
        motorR.drive(80); 
        motorL.drive(80); 
      } 
 
 
    } else if (distancArray[0] == 0 && distancArray[4] != 0) {  // Right sensor is out of range and left sensor is in range 
      int distance = distancArray[4]; 
      if (distance == 0) distance = 15;  // 0 means out of range thus making it 15 - maximum readable distance 
 
      // If the distance is between 7-9, the bot will go straigh. Otherwise necessary adjustments are required. 
      int error = 8 - distance;  // If possitive, the is close to wall - turn right 
                                 // If negative then too far from wall - turn left 
 
      float kp = 10;  // Proportional constant 
 
      float correction = error * kp; 
 
      motorR.drive(70 - correction); 
      motorL.drive(70 + correction); 
 
    } else if (distancArray[4] == 0 && distancArray[0] != 0) {  // Left sensor is out of range and right sensor is in range 
      int distance = distancArray[0]; 
      if (distance == 0) distance = 15;  // 0 means out of range thus making it 15 - maximum readable distance 
 
      // If the distance is between 7-9, the bot will go straigh. Otherwise necessary adjustments are required. 
      int error = 8 - distance;  // If possitive, the is close to wall - turn left 
                                 // If negative then too far from wall - turn right 
 
      float kp = 10;  // Proportional constant 

 
      float correction = error * kp; 
 
      motorR.drive(70 + correction); 
      motorL.drive(70 - correction); 
    } 
 
  } else if (distancArray[2] > 0 && distancArray[2] < fcd) {  // If something infront is detected, decide what to do.... 
    motorL.detach(); 
    motorR.detach(); 
    delay(110);  // Wait for few millisecond so that the distance is updated. 
 
    if (!(distancArray[2] > 0 && distancArray[2] < fcd - 5)) {  // To avoid false detection of object infront 
      motorL.attach(); 
      motorR.attach(); 
      return; 
    } 
 
    motorR.attach(); 
    motorL.attach(); 
 
    // So far we are sure something genuine is present infront . So, lets 
    // check which direction is free and how can we avoid it. 
 
    // Check which direction is free 
    if (distancArray[0] == 0 || distancArray[1] == 0) {  // Right side free 
 
      while (distancArray[2] != 0) {  // Keep turning until the front is free again 

 
        if (distancArray[0] != 0 && distancArray[1] != 0) break;  // The front is still blocked and also 
                                                                  // the right is bloked. Break the loop. 
 
        if (distancArray[0] == 0 && distancArray[1] != 0) {  // Right is free but front-right is blocked 
          motorR.drive(-100);                                // Take a sharp right turn. 
          motorL.drive(100); 
        } else {  // Both right and front-right free or front-right free but right blocked 
                  // Slowly take turn to right 
          motorR.drive(-50); 
          motorL.drive(100); 
        } 
 
        if (distancArray[3] > 0 && distancArray[3] < 5) {  // Bot is colliding on left side 
          // motorR.drive(-100); 
          // motorL.drive(100); 
          // delay(50); 
        } 
 
        delay(5); 
      } 
 
    } else if (distancArray[4] == 0 || distancArray[3] == 0) {  // Left side is free 
 
      while (distancArray[2] != 0) {  // Keep turning until the front is free again 
 
        if (distancArray[4] != 0 && distancArray[3] != 0) break;  // The front is still blocked and also 

 
        if (distancArray[4] == 0 && distancArray[3] != 0) {  // left is free but front-left is blocked 
          motorR.drive(100);                                 // Take a sharp left turn 
          motorL.drive(-100); 
        } else {  // Both left and front-left free or front-left free but left blocked 
                  // Slowly take turn to left 
          motorR.drive(100); 
          motorL.drive(-50); 
        } 
 
        if (distancArray[1] > 0 && distancArray[1] < 5) { 
          // motorR.drive(100); 
          // motorL.drive(-100); 
          // delay(50); 
        } 
 
        delay(5); 
      } 
    } 
  } 
} 
