#include <FastLED.h>
#define NUM_LEDS 60
#define DATA_PIN 6
CRGB leds[NUM_LEDS];

const int motorPinA = 9;
const int motorPinB = 10;
const int motorPinC = 4;
const int motorPinD = 5;
float sentiment = 0;


const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

char messageFromPC[buffSize] = {0};
int newFlashInterval = 0;
float servoFraction = 0.0; // fraction of servo range to move


unsigned long curMillis;

unsigned long prevReplyToPCmillis = 0;
unsigned long replyToPCinterval = 1000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  
  pinMode(motorPinA, OUTPUT);
  pinMode(motorPinB, OUTPUT);
  pinMode(motorPinC, OUTPUT);
  pinMode(motorPinD, OUTPUT);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); 
  Serial.println("<Arduino is ready>");
}

void loop() {
    curMillis = millis();
    getDataFromPC();
    replyToPC();
    resetLEDs();
    FastLED.show();   
}

void resetLEDs() {
    for(int x = 0; x < 60; x++){
        leds[x] = CRGB::Black;
    }
}

void getDataFromPC() {

    // receive data from PC and save it into inputBuffer
    
  if (Serial.available() > 0) {

    char x = Serial.read();

      // the order of these IF clauses is significant
      
    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
      if (sentiment > 0.05) {
        for (int x = 0; x < 60; x++){
          leds[x] = CRGB:: Orange;
        }
      } else if (sentiment > -0.05) {
        for (int x = 0; x < 60; x++){
          leds[x] = CRGB:: SandyBrown;
            digitalWrite(motorPinA, HIGH);
            digitalWrite(motorPinB, LOW);
        }
      } else {
        for (int x = 0; x < 60; x++){
          leds[x] = CRGB:: MidnightBlue;
          digitalWrite(motorPinA, HIGH);
          digitalWrite(motorPinB, LOW);
          digitalWrite(motorPinC, HIGH);
          digitalWrite(motorPinD, LOW);
        }
      }
    }
    
    if(readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }

    if (x == startMarker) { 
      bytesRecvd = 0; 
      readInProgress = true;
    }
  }
}

//=============
 
void parseData() {

    // split the data into its parts
    
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(inputBuffer,",");      // get the first part - the string
  strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC
  
  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  newFlashInterval = atoi(strtokIndx);     // convert this part to an integer
  
  strtokIndx = strtok(NULL, ","); 
  sentiment = atof(strtokIndx);     // convert this part to a float

}

void replyToPC() {

  if (newDataFromPC) {
    newDataFromPC = false;
    Serial.print("<Msg ");
    Serial.print(messageFromPC);
    Serial.print(" NewFlash ");
    Serial.print(newFlashInterval);
    Serial.print(" SrvFrac ");
    Serial.print(servoFraction);
    Serial.print(" Sentiment");
    Serial.print(sentiment);
    Serial.print(" Time ");
    Serial.print(curMillis >> 9); // divide by 512 is approx = half-seconds
    Serial.println(">");
  }
}