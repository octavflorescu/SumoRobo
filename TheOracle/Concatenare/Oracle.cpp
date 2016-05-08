#include "Oracle.h"

#include "Declarations.h"

void oracleSetup() {
  currentState = StateForward;
}

void forwardFullSpeed();

void destroy() {
  if (b == 0) {
    currentState = StateSearch;
    stateTimer = millis();
    powDrt = 0;
    powStg = 0;
    
  } else {
    lastVisibleB = b;
    
    if (b & 0b00011100) {
      if (checkForward()) {
        return;
      }
      
      if (b & 0b00011100 == 0b00011100) {
        powStg = power;
        powDrt = power;
      } else if (b & 0b00011000 == 0b00011000) {
        powStg = power85P;
        powDrt = power;
      } else if (b & 0b00001100 == 0b00001100) {
        powStg = power;
        powDrt = power85P;
      } else if (b & 0b00001000) {  
        powStg = power;
        powDrt = power;
      } else if (b & 0b00010000) {
        powStg = power80P;
        powDrt = power;
      } else {
        powStg = power;
        powDrt = power80P;
      }
    } else if (b & 0b01100000) {
      if (b & 0b00100000) {
        powStg = powerMin;
        powDrt = power40P;
      } else {
        powStg = powerMin;
        powDrt = power60P;
      }
    } else {
      if (b & 0b00000010) {
        powStg = power40P;
        powDrt = powerMin;
      } else {
        powStg = power60P;
        powDrt = powerMin;
      }
    }
  }
}

void rotate() {
    if (b != 0) {
      currentState = StateAttack;
      stateTimer = millis();
      destroy();
      
    } else {
      if (directionDrt) {
        powStg = power;
        powDrt = power70P;
      } else {
        powDrt = power;
        powStg = power70P;
      }
    }
}

void getOnTrack() {
  if (b != 0) {
    currentState = StateAttack;
    stateTimer = millis();
    destroy();
    
  } else if (millis() - stateTimer < 75) {
    if (directionDrt) {
      powStg = power90P;
      powDrt = power70P;
    } else {
      powStg = power70P;
      powDrt = power90P;
    }
      
  } else {
    currentState = StateRotate;
    rotate();
  }
}

void backward() {
  if (b != 0) {
    currentState = StateAttack;
    stateTimer = millis();
    destroy();
    
  } else {
    whiteSensorDrtSaw = analogRead(whiteSensorDrt) >= whiteThreshold;
    whiteSensorStgSaw = analogRead(whiteSensorStg) >= whiteThreshold;
    
    if (!whiteSensorDrtSaw || !whiteSensorStgSaw) { // at least one of the sensors saw
      currentState = StateForward;
      stateTimer = millis();
      forwardFullSpeed();
        
    } else {
      if (directionDrt) {
        powStg = -power50P;
        powDrt = -power80P;
      } else {
        powStg = -power80P;
        powDrt = -power50P;
      }
    }
  }
}

void justSearch() {
  if (b!= 0) {
    currentState = StateAttack;
    stateTimer = millis();
    destroy();
    
  } else if (millis() - stateTimer < 300) {
    if (lastVisibleB & 0b00000111) { // + mid
      powStg = power40P;
      powDrt = powerMin;
      
    } else if (lastVisibleB & 0b01110000) {
      powStg = powerMin;
      powDrt = power40P;
    }
  } else {
    currentState = StateBackward;
    backward();
  }
}

void forwardFullSpeed() {
  if (b != 0) {
    currentState = StateAttack;
    stateTimer = millis();
    destroy();
    
  } else if (millis() - stateTimer < 135) {
    if (directionDrt) {
      powStg = power60P;
      powDrt = power;
    } else {
      powStg = power;
      powDrt = power60P;
    }
      
  } else { // getOnTrack / prepare for rotation
    currentState = StateGetOnTrack;
    stateTimer = millis();
    getOnTrack();
  }
}

void oracleFallback() {
  whiteSensorDrtSaw = analogRead(whiteSensorDrt) >= whiteThreshold;
  whiteSensorStgSaw = analogRead(whiteSensorStg) >= whiteThreshold;
  
  if (!whiteSensorDrtSaw || !whiteSensorStgSaw) {
    currentState = StateForward;
    stateTimer = millis();
    forwardFullSpeed();
    
  } else if (millis() - stateTimer > kTimeWaitFallback) {
    //same as above
    currentState = StateForward;
    stateTimer = millis();
    forwardFullSpeed();
  }
}

void oracleExtraSetup() {
  forwardFullSpeed();
}

void oracleLoop() {
  b = PINB & 0b01111111;

  if (currentState == StateAttack) {
    destroy();
  } else if (currentState == StateForward) {
    forwardFullSpeed();
  } else if (currentState == StateGetOnTrack) {
    getOnTrack();
  } else if (currentState == StateRotate) {
    rotate();
  } else if (currentState == StateSearch) {
    justSearch();
  } else if (currentState == StateBackward) {
    backward();
  } else if (currentState == StateFallbackSide) {
    oracleFallback();
  }
}

