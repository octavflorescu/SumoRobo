#include "Utils.h"
#include "Config.h"

//#define DEBUG

#define startModule 11//pin number for start module
int const whiteSensLeft = A1;
int const whiteSensRight = A0;
int whiteLeft, whiteRight;
boolean onLeftBoth, onRightBoth;

long goForwardTime;

byte  b;

typedef enum state {
  stateForward,
  stateTurnLeft
  //stateWatchYourLeft,
  //stateWatchYourRight,
} State;

int powLeft, powRight;
#ifdef DEBUG
long timerStart; // start stop
#endif
State currentState;
long stateTimer;

void setup() {
  Serial1.begin(57600);
  powLeft = 0;
  powRight= 0;
  
  // READ FROM SENSORS
  DDRB = 0b00000000;
  //DDRF = 0b11111100;

  // START MODULE
  pinAsInput(startModule);
  
  WriteMotors(0, 0);
  delay(100);
  
#ifdef DEBUG
  timerStart = millis();
#endif
  
  while (isLow(startModule)) {
  // DO NOT START UNTIL START/STOP MODULE: STARTED
  // ALL OTHER INIT CODE APART FROM TIMER SHOULD BE ABOVE THIS
#ifdef DEBUG
    if (millis() > timerStart + 5000) {
      break;
    }
#endif
    WriteMotors(0, 0);
  }
  currentState = stateForward;
  //goForwardTime = power * defaultForwardTime / 180;
  powLeft = power;
  powRight = power;

  stateTimer = millis();
}

void loop() 
{
  b = PINB & 0b01111111;

  if (millis() - stateTimer > 300)
  {
    currentState = stateTurnLeft;
    powRight = -power40P;
    powLeft = -power60P;
    //stateTimer = millis();
  }

  switch (b)
  {
    
    case 0b01100000:
      //onLeftBoth = TRUE;
      powLeft = power60P;
      powRight = power;
      break;
    case 0b01000000:
      //currentState = stateWatchYourLeft;
      powLeft = power55P;
      powRight = power;
      break;
    case 0b00100000:
      //currentState = stateWatchYourLeft;
      powLeft = power65P;
      powRight = power;
      break;
    case 0b00010000:
      powLeft = power80P;
      powRight = power;
      break;

    case 0b00001100:
      powLeft = power90P;
      powRight = power;

    case 0b00011000:
      powLeft = power;
      powRight = power90P;

    case 0b00011100:
      powLeft = power;
      powRight = power;

    case 0b00001000:
      powLeft = power90P;
      powRight = power;
      break;

    case 0b00000100:
      powLeft = power;
      powRight = power80P;
      break;
    case 0b00000011:
      powLeft = power;
      powRight = power60P;
      //onRightBoth = TRUE;
      break;
    case 0b00000010:
      //currentState = stateWatchYourRight;
      powLeft = power;
      powRight = power65P;
      //stateTimer = millis();
      break;
    case 0b00000001:
      //currentState = stateWatchYourRight;
      powLeft = power;
      powRight = power55P;
      //stateTimer = millis();
      break;
      
    default:    //no sensor sees
      whiteLeft = analogRead(whiteSensLeft);
      whiteRight = analogRead(whiteSensRight);
      if(currentState == stateTurnLeft)
      {
        if ((whiteRight <= whiteThreshold)
          || (whiteLeft <= whiteThreshold))
          {
            currentState = stateForward;
            powLeft = power;
            powRight = power;
            stateTimer = millis();
          }
      }
      break;
  }
  
//  switch (currentState) 
//  {
//    case stateForward:
//      goForward();
//      break;
//      
//    case stateTurnLeft:
//      turnLeftUntilWhiteLine();
//      break;

//    case stateWatchYourLeft:
//      watchLeft();
//      break;
//
//    case stateWatchYourRight:
//      watchRight();
//      break;
      
//    default:
//      
//      break;
//  }
  
  WriteMotors(powLeft, powRight);
}

void turnLeftUntilWhiteLine()
{
  if ((whiteRight <= whiteThreshold)
  || (whiteLeft <= whiteThreshold))
  {
    currentState = stateForward;
    stateTimer = millis();
  }
}


