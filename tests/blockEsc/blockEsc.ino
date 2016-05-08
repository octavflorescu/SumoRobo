#include "Utils.h"
#include "Config.h"
// faceBlockEsc

#define startModule 11//pin number for start module
// senzor alb STG A1
// senzor alb DRT A0

//#define activateAttack

typedef enum state {
  StateForward,
  StateFallback,
  StateFallbackSide,
  StateNULL
} State;

int powStg, powDrt;
State currentState;
long stateTimer;
bool isLeft;
byte b;


void setup() {
  Serial1.begin(57600);
  powStg = 0;
  powDrt = 0;
  DDRB = 0b00000000;
  currentState = StateForward;
  isLeft = random(2) == 1 ? true : false;
  
  pinAsInput(startModule);
  
  WriteMotors(0, 0);
  delay(50);
  WriteMotors(0, 0);
  delay(50);
  
  while (isLow(startModule)) {
    WriteMotors(0, 0);
  }
  stateTimer = millis();
  forwardWhileChecking();
}

void loop() {
  switch (currentState) {
    case StateForward:
      forwardWhileChecking();
      break;
      
    case StateFallback:
      fallback();
      break;

    case StateFallbackSide:
      sideFallback();
      break;
      
    default:
      break;
  }
  
  WriteMotors(powStg, powDrt);
}

void forwardWhileChecking() {
  b = PINB & 0b00011100;
  if (b & 0b00001000) {
    if (millis() - stateTimer > 300) {
      if (b == 0b00011100) {
        currentState = StateFallbackSide;
        
        if (directionDrt) {
          powDrt = -power50P;
          powStg = power;
          
        } else {
          powStg = -power50P;
          powDrt = power;
        }
        stateTimer = millis();
        
      } else if (b == 0b00011000) {
        currentState = StateFallbackSide;
        powStg = -power50P;
        powDrt = power;
        stateTimer = millis();
        
      } else if (b == 0b00001100) {
        stateTimer = millis();
        currentState = StateFallbackSide;
        powDrt = -power50P;
        powStg = power;
        stateTimer = millis();
        
      } else { // b == mid only
        powDrt = power;
        powStg = power;
        stateTimer = millis() - 275;
      }
    } else {
#ifdef activateAttack
      focus(b);
#else
      powStg = power;
      powDrt = power;
#endif
    }
  } else {
    powStg = 0;
    powDrt = 0;
  }
}

void fallback() {
  // senzor alb STG A1
  // senzor alb DRT A0
//  if (analogRead(A1) < whiteThreshold || analogRead(A0) < whiteThreshold) {
//    
//#ifdef activateAttack
//    focus(PINB & 0b00011100);
//#else
//    powStg = 0;
//    powDrt = 0;
//    currentState = StateNULL;
//#endif
//    
//  } else
  if (millis() - stateTimer > 200) {
    currentState = StateNULL;
    powDrt = 0;
    powStg = 0;
  }
}

void sideFallback() {
  // senzor alb STG A1
  // senzor alb DRT A0
//  if (analogRead(A0) < whiteThreshold || analogRead(A1) < whiteThreshold) {
    
//#ifdef activateAttack
//    focus(PINB & 0b00011100);
//#else
//    powStg = 0;
//    powDrt = 0;
//    currentState = StateNULL;
//#endif

//  } else 
  if (millis() - stateTimer > 350) {
    currentState = StateNULL;
    powDrt = 0;
    powStg = 0;
  }
}


#ifdef activateAttack

void focus(byte b) {
  switch(b) {
    case 0b00011100:
    case 0b00001000:
      powDrt = power;
      powStg = power;
      break;
      
    case 0b00011000:
      powStg = power85P;
      powDrt = power;
      break;
      
    case 0b00001100:
      powStg = power;
      powDrt = power85P;
      break;

    default:
      powDrt = 0;
      powStg = 0;
    break;
  }
}

#endif

