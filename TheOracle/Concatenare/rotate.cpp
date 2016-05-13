#include "rotate.h"

#include "Declarations.h"

bool didAttackAtLeastOnce = false;

void rotateSetup() {
  currentState = StateSearch;
  backTimer = 0;
}

void rotateExtraSetup() {
    backTimer = millis();
}

// to be called only if b & 0b00001000
void rotateAttack() {
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

void rotateSearch() {
  switch (PINB & 0b01111111) {
    case 0b00011000:
      currentState = StateAttack;
      powStg = power90P;
      powDrt = power;
      stateTimer = millis();
      break;
      
    case 0b00001100:
      currentState = StateAttack;
      powStg = power;
      powDrt = power90P;
      stateTimer = millis();
      break;
      
    case 0b00011100:
    case 0b00001000:
      currentState = StateAttack;
      powStg = power;
      powDrt = power;
      stateTimer = millis();
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
      
    default: // no check was successful ( b == 0)
      if (didAttackAtLeastOnce) {
      whiteSensorDrtSaw = analogRead(whiteSensorDrt) >= whiteThreshold;
      whiteSensorStgSaw = analogRead(whiteSensorStg) >= whiteThreshold;
      
      if (whiteSensorDrtSaw && whiteSensorStgSaw) {
        if (backTimer == 0) {
          backTimer = millis();
          powStg = 0;
          powDrt = 0;
          
        } else if (millis() - backTimer > 500) {
          stateTimer = millis();
          powStg = -power;
          powDrt = -power;
        }
      } else {
        runTime = (millis() - stateTimer) / 2;
        
        if (whiteSensorDrtSaw == whiteSensorStgSaw) { // == false
          currentState = StateGetOnTrack;
          powStg = power;
          powDrt = power;
          
        } else if (whiteSensorDrtSaw) { // the opposite
          currentState = StateGetOnTrack;
          powStg = power90P;
          powDrt = power;
          
        } else { // whiteSensorDrtSaw // the opposite
          currentState = StateGetOnTrack;
          powStg = power;
          powDrt = power90P;
        }
        stateTimer = millis();
      }
      } else {
        
        if (millis() - backTimer > 400) {
          currentState = StateGetOnTrack;
          powStg = power;
          powDrt = power;
          stateTimer = millis();
        }
      }
      break;
  }
}

void rotateFallback() {
  whiteSensorDrtSaw = analogRead(whiteSensorDrt) >= whiteThreshold;
  whiteSensorStgSaw = analogRead(whiteSensorStg) >= whiteThreshold;
  
  if (!whiteSensorDrtSaw || !whiteSensorStgSaw) {
    currentState = StateSearch;
    powStg = 0;
    powDrt = 0;
    didAttackAtLeastOnce = true;
    stateTimer = millis();

  } else if (millis() - stateTimer > kTimeWaitFallback) {
    //same as above
    currentState = StateSearch;
    powStg = 0;
    powDrt = 0;
    didAttackAtLeastOnce = true;
    stateTimer = millis();
  }
}

void rotateLoop() {
  switch (currentState) {
    case StateAttack:
      b = PINB & 0b00011100;
      
      if (b & 0b00001000 == 0) {
        powStg = 0;
        powDrt = 0;
        currentState = StateSearch;
        didAttackAtLeastOnce = true;
        
      } else {
        if (checkForward()) {
          return;
        }
        rotateAttack();
      }
      break;
    
    case StateSearch:
      rotateSearch();
      break;
      
    case StateRotateSideBack:
      if (millis() - stateTimer > kTimeForSideBack) {
        powStg = 0;
        powDrt = 0;
        currentState = StateSearch;
      }
      break;
      
    case StateRotateSideFront:
      if (millis() - stateTimer > kTimeForSideFront) {
        powStg = 0;
        powDrt = 0;
        currentState = StateSearch;
      }
      break;
      
    case StateRotateFrontSide:
      b = PINB & 0b00011100;
      
      if (b & 0b00001000) {
        currentState = StateAttack;
        rotateAttack();
        stateTimer = millis();
        
      } else if (millis() - stateTimer > kTimeForFront) {
        powStg = 0;
        powDrt = 0;
        currentState = StateSearch;
      }
      break;

    case StateGetOnTrack:
      b = PINB & 0b01111111;
      
      if (b & 0b00001000) {
        currentState = StateAttack;
        backTimer = 0;
        rotateAttack();
        stateTimer = millis();
        
      } else if (millis() - stateTimer > runTime) {
        powStg = 0;
        powDrt = 0;
        backTimer = 0;
        currentState = StateSearch;
      }
      break;
      
    case StateFallbackSide:
      rotateFallback();
      break;
          
    default:
      rotateSearch();
      break;
  }
}

