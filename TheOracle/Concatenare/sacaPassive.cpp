#include "Declarations.h"

#define kPassiveWaitTime 2000
#define kPassiveGoingForwardCounts 10

void sacaPassiveSetup() {
  forwardCounts = 0;
  
  currentState = StateSearch;
}

void sacaPassiveExtraSetup() {
  
}

void passiveForward() {
  if (millis() - stateTimer > 44) {
    currentState = StateSearch;
    powStg = 0;
    powDrt = 0;
    stateTimer = millis();
  }
}

void passiveHalt() {
  if (millis() - stateTimer > 80) {
    currentState = StateSearch;
    forwardCounts = 0;
    powStg = 0;
    powDrt = 0;
    stateTimer = millis();
  }
}

void passiveSearch() {
    if (forwardCounts >= kPassiveGoingForwardCounts) {
        currentState = StateHalt;
        
        if (((int)&forwardCounts) % 2 == 1) {
            powDrt = -power;
            powStg = power;
            
        } else {
            powStg = -power;
            powDrt = power;
        }
        stateTimer = millis();
        
    } else if (millis() - stateTimer > kPassiveWaitTime) {
        currentState = StateForward;
        forwardCounts++;
        powDrt = power;
        powStg = power;
        stateTimer = millis();
        
    } else {
        powStg = 0;
        powDrt = 0;
    }
}

void sacaPassiveLoop() {
  switch (currentState) {
    case StateForward:
      passiveForward();
    break;
    
    case StateSearch:
      passiveSearch();
    break;
    
    case StateHalt:
      passiveHalt();
    break;
    
    default:
    break;
  }
}
