#include "Utils.h"
#include "Config.h"

#define startModule 11//pin number for start module
int const whiteSensorStg = A1;
int const whiteSensorDrt = A0;

#define kWaitTime 640
#define kSeeingForwardCounts 2
#define kGoingForwardCounts 10

typedef enum state {
  StateAttack,
  StateForward,
  StateSearch,
  StateRotateSideBack,
  StateRotateSideFront,
  StateRotateFrontSide,
  StateHalt
} State;

int powStg, powDrt;
State currentState;
long stateTimer;
short forwardCounts;
short seeingForwardCounts;
byte b;

void setup() {
  Serial1.begin(57600);
  forwardCounts = 0;
  seeingForwardCounts = 0;
  DDRB = 0b00000000;
  powStg = 0;
  powDrt = 0;
  currentState = StateSearch;
  
  pinAsInput(startModule);
  WriteMotors(0, 0);
  delay(50);
  WriteMotors(0, 0);
  delay(50);
  
  while (isLow(startModule)) {
    WriteMotors(0, 0);
  }
  stateTimer = millis();
}

void loop() {
  switch (currentState) {
    case StateAttack:
      attack();
    break;
    
    case StateForward:
      forward();
    break;
    
    case StateSearch:
      search();
    break;
    
    case StateRotateSideBack:
      if (millis() - stateTimer > kTimeForSideBack) {
        powStg = 0;
        powDrt = 0;
        currentState = StateSearch;
        seeingForwardCounts = 0;
        stateTimer = millis();
      }
    break;
    
    case StateRotateSideFront:
      if (millis() - stateTimer > kTimeForSideFront) {
        powStg = 0;
        powDrt = 0;
        currentState = StateSearch;
        seeingForwardCounts = 0;
        stateTimer = millis();
      }
    break;
    
    case StateRotateFrontSide:
      if (millis() - stateTimer > kTimeForFront) {
        powStg = 0;
        powDrt = 0;
        currentState = StateSearch;
        seeingForwardCounts = 0;
        stateTimer = millis();
      }
    break;
    
    case StateHalt:
      halt();
    break;
    
    default:
    break;
  }
  
  WriteMotors(powStg, powDrt);
}

void search() {
  b = PINB & 0b01111111;
  
  switch (b) {
    case 0:
      seeingForwardCounts = 0;
      
      if (forwardCounts >= kGoingForwardCounts) {
        currentState = StateHalt;
        
        if (random(2) == 1) {
          powDrt = -power;
          powStg = power;
          
        } else {
          powStg = -power;
          powDrt = power;
        }
        stateTimer = millis();
        
      } else if (millis() - stateTimer > kWaitTime) {
        currentState = StateForward;
        forwardCounts++;
        powDrt = power;
        powStg = power;
        stateTimer = millis();
        
      } else {
        powStg = 0;
        powDrt = 0;
      }
      break;
    
    case 0b01000000:
      currentState = StateRotateSideBack;
      powStg = -power;
      powDrt = power;
      stateTimer = millis();
      break;
      
    case 0b01100000:
    case 0b00100000:
      currentState = StateRotateSideFront;
      powStg = -power;
      powDrt = power;
      stateTimer = millis();
      break;

    case 0b00110000: // front stg + stg front
      currentState = StateRotateFrontSide;
      powStg = power75P;
      powDrt = power;
      stateTimer = millis();
      break;

    case 0b00010000:
      currentState = StateRotateFrontSide;
      powStg = power80P;
      powDrt = power;
      stateTimer = millis();
      break;

    case 0b00000110: // front drt + drt front
      currentState = StateRotateFrontSide;
      powStg = power;
      powDrt = power75P;
      stateTimer = millis();
      break;
    
    case 0b00000100:
      currentState = StateRotateFrontSide;
      powStg = power;
      powDrt = power80P;
      stateTimer = millis();
      break;

    case 0b00000011:
    case 0b00000010:
      currentState = StateRotateSideFront;
      powStg = power;
      powDrt = -power;
      stateTimer = millis();
      break;

    case 0b00000001:
      currentState = StateRotateSideBack;
      powStg = power;
      powDrt = -power;
      stateTimer = millis();
      break;

    case 0b00011100:
      currentState = StateAttack;
      powStg = power;
      powDrt = power;
      stateTimer = millis();
      break;

    case 0b00001000:
      powStg = power;
      powDrt = power;
      
      if (seeingForwardCounts  >= kSeeingForwardCounts) {
        currentState = StateAttack;
        
      } else if (millis() - stateTimer > kWaitTime) {
        currentState = StateForward;
        seeingForwardCounts++;
        forwardCounts++;
        stateTimer = millis();
        
      } else {
        powStg = 0;
        powDrt = 0;
      }
      break;

    case 0b00011000:
      powStg = power90P;
      powDrt = power;
      
      if (seeingForwardCounts >= kSeeingForwardCounts) {
        currentState = StateAttack;
        
      } else if (millis() - stateTimer > kWaitTime) {
        currentState = StateForward;
        seeingForwardCounts++;
        forwardCounts++;
        stateTimer = millis();
        
      } else {
        powStg = 0;
        powDrt = 0;
      }
      break;

    case 0b00001100:
      powStg = power;
      powDrt = power90P;
      
      if (seeingForwardCounts  >= kSeeingForwardCounts) {
        currentState = StateAttack;
        
      } else if (millis() - stateTimer > kWaitTime) {
        currentState = StateForward;
        seeingForwardCounts++;
        forwardCounts++;
        stateTimer = millis();
        
      } else {
        powStg = 0;
        powDrt = 0;
      }
      break;
      
    default: // sees with the mid one
      break;
  }
}

void attack() {
  b = PINB & 0b00011100;
  
  if (b & 0b00001000 == 0) {
    seeingForwardCounts = 0;
    forwardCounts = kGoingForwardCounts;
    currentState = StateSearch;
    powStg = 0;
    powDrt = 0;
    stateTimer = millis();
    
  } else if (b == 0b00011000) {
    powStg = power90P;
    powDrt = power;
    
  } else if (b == 0b00001100) {
    powDrt = power90P;
    powStg = power;
    
  } else {
    powStg = power;
    powDrt = power;
  }
}

void forward() {
  if (millis() - stateTimer > 44) {
    currentState = StateSearch;
    powStg = 0;
    powDrt = 0;
    stateTimer = millis();
  }
}

void halt() {
  if (millis() - stateTimer > 80) {
    currentState = StateSearch;
    forwardCounts = 0;
    seeingForwardCounts = 0;
    powStg = 0;
    powDrt = 0;
    stateTimer = millis();
  }
}

