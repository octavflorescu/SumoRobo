#include "Utils.h"
#include "Config.h"

//#define DEBUG

#define startModule 11//pin number for start module
int const whiteSensorStg = A1;
int const whiteSensorDrt = A0;
bool whiteSensorDrtSaw;
bool whiteSensorStgSaw;

typedef enum state {
  StateAttack,
  StatePause,
  StateRotateStgBack, // lateral spate
  StateRotateStgFront, // lateral fata
  StateRotateFrontStg,
  StateRotateDrtBack, // lateral spate
  StateRotateDrtFront, // lateral fata
  StateRotateFrontDrt,
  StateRun
} State;

int powStg, powDrt;
#ifdef DEBUG
long timerStart; // start stop
#endif
State currentState;
byte stateB;
long stateTimer;
long backTimer;
long runTime;

void setup() {
  Serial1.begin(57600);
  powStg = 0;
  powDrt = 0;
  currentState = StatePause;
  backTimer = 0;
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
}

void loop() {
  switch (currentState) {
    case StateAttack:
      stateB = PINB & 0b00011100;
      
      if (stateB & 0b00001000 == 0) {
        powStg = 0;
        powDrt = 0;
        currentState = StatePause;
        
      } else {
        attack();
      }
      break;
    
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
      if (PINB & 0b00001000) {
        powStg = power90P;
        powDrt = power;
        currentState = StateAttack;
        
      } else if (millis() - stateTimer > kTimeForFront) {
        powStg = 0;
        powDrt = 0;
        currentState = StatePause;
      }
      break;
      
    case StateRotateFrontDrt:
      if (PINB & 0b00001000) {
        powStg = power;
        powDrt = power90P;
        currentState = StateAttack;
        
      } else if (millis() - stateTimer > kTimeForFront) {
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

    case StateRun:
      stateB = PINB & 0b01111111;
      
      if (stateB & 0b00001000) {
        currentState = StateAttack;
        attack();
        
      } else if (millis() - stateTimer > runTime) {
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
  switch (PINB & 0b01111111) {
    case 0b00011000:
      currentState = StateAttack;
      powStg = power90P;
      powDrt = power;
      break;
      
    case 0b00001100:
      currentState = StateAttack;
      powStg = power;
      powDrt = power90P;
      break;
      
    case 0b00011100:
    case 0b00001000:
      currentState = StateAttack;
      powStg = power;
      powDrt = power;
      break;
    
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
      
    default: // no check was successful ( b == 0)
      whiteSensorDrtSaw = analogRead(whiteSensorDrt) >= whiteThreshold;
      whiteSensorStgSaw = analogRead(whiteSensorStg) >= whiteThreshold;
      
      if (whiteSensorDrtSaw && whiteSensorStgSaw) {
        if (backTimer == 0) {
          backTimer = millis();
          powStg = 0;
          powDrt = 0;
          
        } else if (millis() - backTimer > 250) {
          stateTimer = millis();
          powStg = -power;
          powDrt = -power;
        }
      } else {
        runTime = millis() - stateTimer;
        backTimer = 0;
        
        if (whiteSensorDrtSaw == whiteSensorStgSaw) { // == false
          currentState = StateRun;
          powStg = power;
          powDrt = power;
          
        } else if (whiteSensorDrtSaw) { // the opposite
          currentState = StateRun;
          powStg = power90P;
          powDrt = power;
          
        } else { // whiteSensorDrtSaw // the opposite
          currentState = StateRun;
          powStg = power;
          powDrt = power90P;
        }
        stateTimer = millis();
      }
      break;
  }
}

// to be called only if stateB & 0b00001000
void attack() {
  if (stateB == 0b00011100 || stateB == 0b00001000) {
    powStg = power;
    powDrt = power;
        
  } else if (stateB == 0b00011000) { // stg
    powStg = power90P;
    powDrt = power;
        
  } else { // drt
    powStg = power;
    powDrt = power90P;
  }
}

