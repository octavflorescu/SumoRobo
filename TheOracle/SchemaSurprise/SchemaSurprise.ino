#include "Utils.h"
#include "Config.h"

#define kTimeRotateMicros 50000
#define kTimeRelease 35000

#define kTimeForSideBack 30000
#define kTimeForSideFront 10000
#define kTimeForFront 5000

typedef enum {
  StateBack,
  StateRotate,
  StateSearch,
  StateCamp,
  StateAim,
  StateAttack
} State;

typedef enum {
  AimStateStgBack, // lateral spate
  AimStateStgFront, // lateral fata
  AimStateFrontStg,
  AimStateDrtBack, // lateral spate
  AimStateDrtFront, // lateral fata
  AimStateFrontDrt
} AimState;

byte b, sideB, oldSideB;
int powStg, powDrt;
State currentState;
AimState aimState;
unsigned long timer;
unsigned long sawTimer;

void setup() {
  Serial1.begin(57600);
  powStg = 0;
  powDrt = 0;
  DDRB = 0b00000000;
  b = 0;
  sideB = 0;
  oldSideB = 0;
  sawTimer = 0;
  
  pinAsInput(startModule);
  WriteMotors(0, 0);
  delay(100);
  
  while (isLow(startModule)) {
    WriteMotors(0, 0);
  }
  timer = micros();
  while (getOnTrack()) {
    WriteMotors(powStg, powDrt);
  }
}

void loop() {
  b = PINB & 0b01111111;
  
  switch (currentState) {
    case StateAttack:
      if (b & 0b00001000 == 0) {
        powStg = 0;
        powDrt = 0;
        currentState = StateCamp;
        
      } else {
        attack();
      }
      break;
      
    case StateBack:
      if (b & 0b00001000) {
        currentState = StateAttack;
        attack();
        
      } else if (b) {
        currentState = StateCamp;
        powStg = 0;
        powDrt = 0;
        
      } else {
        backward();
      }
      break;

    case StateRotate:
      getOnTrack();
      break;

    case StateSearch: //circle rotate
      search();
      break;

    case StateCamp:
      camp();
      break;

    case StateAim:
      aim();
      break;
      
    default:
      powStg = 0;
      powDrt = 0;
      break;
  }
  WriteMotors(powStg, powDrt);
}

// to be called only if stateB & 0b00001000
void attack() {
  if (b == 0b00011100 || b == 0b00001000) {
    powStg = power;
    powDrt = power;
        
  } else if (b == 0b00011000) { // stg
    powStg = power90P;
    powDrt = power;
        
  } else { // drt
    powStg = power;
    powDrt = power90P;
  }
}

bool getOnTrack() {
  if (b == 0) {
    if (micros() - timer < kTimeRotateMicros) {
#ifdef rotateSTG
      powStg = power;
      powDrt = -power;
#else
      powStg = -power;
      powDrt = power;
#endif
      return true;
    }
    currentState = StateSearch;
#ifdef rotateSTG
    powStg = power85P;
    powDrt = power;
#else
    powStg = power;
    powDrt = power85P;
#endif
    return false;
  }
  currentState = StateCamp;
  powStg = 0;
  powDrt = 0;
  
  return false;
}

bool whiteSensorDrtIsBlack;
bool whiteSensorStgIsBlack;

void backward() {
  whiteSensorDrtIsBlack = analogRead(whiteSensorDrt) >= whiteThreshold;
  whiteSensorStgIsBlack = analogRead(whiteSensorStg) >= whiteThreshold;
      
  if (whiteSensorDrtIsBlack && whiteSensorStgIsBlack) {
#ifdef rotateSTG
    powStg = -power90P;
    powDrt = -power;
#else
    powStg = -power;
    powDrt = -power90P;
#endif
    
  } else {
    if (whiteSensorDrtIsBlack == whiteSensorStgIsBlack) { // isWhite
      currentState = StateRotate;
      timer = micros();
      getOnTrack();
          
    } else if (whiteSensorDrtIsBlack) {
      currentState = StateRotate;
      powStg = -power50P;
      powDrt = 0;
      timer = micros() + 4;
          
    } else {
      currentState = StateRotate;
      powStg = 0;
      powDrt = -power50P;
      timer = micros() + 4;
    }
  }
}

void search() {
  if (b & 0b00001000) { //(sideB & 0b00001000) {
    currentState = StateAttack;
    attack();
        
  } else if (b != 0) {//if (sawTimer > 0) {
//    if (micros() - sawTimer > kTimeRelease) {
//      sawTimer = 0;
//      sideB = 0;
//      oldSideB = 0;
      
      currentState = StateCamp;
      powStg = 0;
      powDrt = 0;
      
//    } else if (sideB & oldSideB == 0 && sideB & (oldSideB << 1) == 0 && (sideB << 1) & oldSideB== 0) {
//      if (sideB) {
//        oldSideB = sideB;
//        sawTimer = micros();
//        
//      } else {
//        oldSideB = 0b11111111;
//        sawTimer = 0;
//      }
//    }
//  } else if (sideB) { // && sawTimer == 0
//    oldSideB = sideB;
//    sawTimer = micros();
  }
}

void camp() {
  currentState = StateAim;
  
  switch (b) {
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
      aimState = AimStateStgBack;
      powStg = -power;
      powDrt = power;
      sawTimer = micros();
      break;
      
    case 0b01100000:
    case 0b00100000:
      aimState = AimStateStgFront;
      powStg = -power;
      powDrt = power;
      sawTimer = micros();
      break;

    case 0b00110000: // front stg + stg front
      aimState = AimStateFrontStg;
      powStg = power75P;
      powDrt = power;
      sawTimer = micros();
      break;

    case 0b00010000:
      aimState = AimStateFrontStg;
      powStg = power80P;
      powDrt = power;
      sawTimer = micros();
      break;

    case 0b00000110: // front drt + drt front
      aimState = AimStateFrontDrt;
      powStg = power;
      powDrt = power75P;
      sawTimer = micros();
      break;
    
    case 0b00000100:
      aimState = AimStateFrontDrt;
      powStg = power;
      powDrt = power80P;
      sawTimer = micros();
      break;

    case 0b00000011:
    case 0b00000010:
      aimState = AimStateDrtFront;
      powStg = power;
      powDrt = -power;
      sawTimer = micros();
      break;

    case 0b00000001:
      aimState = AimStateDrtBack;
      powStg = power;
      powDrt = -power;
      sawTimer = micros();
      break;
      
    default: // no check was successful ( b == 0)
      currentState = StateBack;
      break;
  }
}

void aim() {
  switch (aimState) {
    case AimStateStgBack:
      if (micros() - sawTimer > kTimeForSideBack) {
        powStg = 0;
        powDrt = 0;
        sawTimer = 0;
        currentState = StateCamp;
      }
      break;
      
    case AimStateStgFront:
      if (micros() - sawTimer > kTimeForSideFront) {
        powStg = 0;
        powDrt = 0;
        sawTimer = 0;
        currentState = StateCamp;
      }
      break;
      
    case AimStateFrontStg:
      if (b & 0b00001000) {
        powStg = power90P;
        powDrt = power;
        sawTimer = 0;
        currentState = StateAttack;
        
      } else if (micros() - sawTimer > kTimeForFront) {
        powStg = 0;
        powDrt = 0;
        sawTimer = 0;
        currentState = StateCamp;
      }
      break;
      
    case AimStateFrontDrt:
      if (b & 0b00001000) {
        powStg = power;
        powDrt = power90P;
        sawTimer = 0;
        currentState = StateAttack;
        
      } else if (micros() - sawTimer > kTimeForFront) {
        powStg = 0;
        powDrt = 0;
        sawTimer = 0;
        currentState = StateCamp;
      }
      break;
      
    case AimStateDrtFront:
      if (micros() - sawTimer > kTimeForSideFront) {
        powStg = 0;
        powDrt = 0;
        sawTimer = 0;
        currentState = StateCamp;
      }
      break;
      
    case AimStateDrtBack:
      if (micros() - sawTimer > kTimeForSideBack) {
        powStg = 0;
        powDrt = 0;
        sawTimer = 0;
        currentState = StateCamp;
      }
      break;
    
    default:
      break;
  }
}

