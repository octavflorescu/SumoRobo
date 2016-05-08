#include "Utils.h"
#include "Config.h"

//#define DEBUG

#define startModule 11//pin number for start module
//int const whiteSensorStg = A1;
//int const whiteSensorDrt = A0;

typedef enum state {
  StatePause,
  StateRotateStgBack, // lateral spate
  StateRotateStgFront, // lateral fata
  StateRotateFrontStg,
  StateRotateDrtBack, // lateral spate
  StateRotateDrtFront, // lateral fata
  StateRotateFrontDrt
} State;

int powStg, powDrt;
#ifdef DEBUG
long timerStart; // start stop
#endif
State currentState;
long stateTimer;

void setup() {
  Serial1.begin(57600);
  powStg = 0;
  powDrt = 0;
  // READ FROM SENSORS
  DDRB = 0b00000000;

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
  currentState = StatePause;
}

void loop() {
  switch (currentState) {
    case StatePause:
      search();
      break;
      
    case StateRotateStgBack:
      if (millis() - stateTimer > kTimeForSideBack) {
        powStg = 0;
        powDrt = 0;
        currentState = StatePause;
      }
      break;
      
    case StateRotateStgFront:
      if (millis() - stateTimer > kTimeForSideFront) {
        powStg = 0;
        powDrt = 0;
        currentState = StatePause;
      }
      break;
      
    case StateRotateFrontStg:
      if (millis() - stateTimer > kTimeForFront) {
        powStg = 0;
        powDrt = 0;
        currentState = StatePause;
      }
      break;
      
    case StateRotateFrontDrt:
      if (millis() - stateTimer > kTimeForFront) {
        powStg = 0;
        powDrt = 0;
        currentState = StatePause;
      }
      break;
      
    case StateRotateDrtFront:
      if (millis() - stateTimer > kTimeForSideFront) {
        powStg = 0;
        powDrt = 0;
        currentState = StatePause;
      }
      break;
      
    case StateRotateDrtBack:
      if (millis() - stateTimer > kTimeForSideBack) {
        powStg = 0;
        powDrt = 0;
        currentState = StatePause;
      }
      break;
      
    default:
      break;
  }
  
  WriteMotors(powStg, powDrt);
}

void search() {
  switch (PINB & 0b01100011) {//0b01111111) {
    case 0b01000000:
      currentState = StateRotateStgBack;
      powStg = -power;
      powDrt = power;
      stateTimer = millis();
      break;
      
    case 0b01100000:
    case 0b00100000:
      currentState = StateRotateStgFront;
      powStg = -power;
      powDrt = power;
      stateTimer = millis();
      break;

    case 0b00110000: // front stg + stg front
      currentState = StateRotateFrontStg;
      powStg = power75P;
      powDrt = power;
      stateTimer = millis();
      break;

    case 0b00010000:
      currentState = StateRotateFrontStg;
      powStg = power80P;
      powDrt = power;
      stateTimer = millis();
      break;

    case 0b00000110: // front drt + drt front
      currentState = StateRotateFrontDrt;
      powStg = power;
      powDrt = power75P;
      stateTimer = millis();
      break;
    
    case 0b00000100:
      currentState = StateRotateFrontDrt;
      powStg = power;
      powDrt = power80P;
      stateTimer = millis();
      break;

    case 0b00000011:
    case 0b00000010:
      currentState = StateRotateDrtFront;
      powStg = power;
      powDrt = -power;
      stateTimer = millis();
      break;

    case 0b00000001:
      currentState = StateRotateDrtBack;
      powStg = power;
      powDrt = -power;
      stateTimer = millis();
      break;
      
    default:
      break;
  }
}

