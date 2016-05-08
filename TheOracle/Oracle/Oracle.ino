#include "Utils.h"
#include "Config.h"

//#define DEBUG

#define startModule 11//pin number for start module
int const whiteSensorStg = A1;
int const whiteSensorDrt = A0;

typedef enum state {
  StateForward,
  StateGetOnTrack,
  StateRotate,
  StateBackward,
  StateDestroy,
  StateJustSearch
} State;

byte b;
byte lastVisibleB;
int powStg, powDrt;
State currentState;
long stateTimer;

void setup() {
  Serial1.begin(57600);
  powStg = 0;
  powDrt = 0;
  DDRB = 0b00000000;

  // START MODULE
  pinAsInput(startModule);
  WriteMotors(0, 0);
  delay(100);
  
  while (isLow(startModule)) {
    WriteMotors(0, 0);
  }

  currentState = StateForward;
  stateTimer = millis();
  // we are sure that we want to go forward first
  b = 0;
  forwardFullSpeed();
  WriteMotors(powStg, powDrt);
}

void loop() {
  b = PINB & 0b01111111;

  if (currentState == StateDestroy) {
    destroy();
  } else if (currentState == StateForward) {
    forwardFullSpeed();
  } else if (currentState == StateGetOnTrack) {
    getOnTrack();
  } else if (currentState == StateRotate) {
    rotate();
  } else if (currentState == StateJustSearch) {
    justSearch();
  } else if (currentState == StateBackward) {
    backward();
  }
  WriteMotors(powStg, powDrt);
}

bool destroy() {
  if (b == 0) {
    currentState = StateJustSearch;
    stateTimer = millis();
    powDrt = 0;
    powStg = 0;
    
  } else {
    lastVisibleB = b;
    
    if (b & 0b00011100) {
      if (b & 0b00011100 == 0b00011100){
        powStg = power;
        powDrt = power;
      } else if (b & 0b00011000 == 0b00011000){
        powStg = power85P;
        powDrt = power;
      } else if (b & 0b00001100 == 0b00001100){
        powStg = power;
        powDrt = power85P;
      } else if (b & 0b00001000){  
        powStg = power;
        powDrt = power;
      } else if (b & 0b00010000) {
        powStg = power80P;
        powDrt = power;
      } else {
        powStg = power;
        powDrt = power80P;
      }
    } else if (b & 0b01100000){
      if (b & 0b00100000) {
        powStg = powerMinForward;
        powDrt = power40P;
      } else {
        powStg = powerMinForward;
        powDrt = power60P;
      }
    } else {
      if (b & 0b00000010) {
        powStg = power40P;
        powDrt = powerMinForward;
      } else {
        powStg = power60P;
        powDrt = powerMinForward;
      }
    }
  }
}

void forwardFullSpeed() {
  if (b != 0) {
    currentState = StateDestroy;
    destroy();
    
  } else if (millis() - stateTimer < 135) {
      powStg = power60P;
      powDrt = power;
      
  } else { // getOnTrack / prepare for rotation
    currentState = StateGetOnTrack;
    stateTimer = millis();
    getOnTrack();
  }
}

void getOnTrack() {
  if (b != 0) {
    currentState = StateDestroy;
    destroy();
    
  } else if (millis() - stateTimer < 75) {
    powStg = power90P;
    powDrt = power70P;
      
  } else {
    currentState = StateRotate;
    rotate();
  }
}

void rotate() {
    if (b != 0) {
      currentState = StateDestroy;
      destroy();
      
    } else {
      powStg = power;
      powDrt = power70P;
    }
}

void justSearch() {
  if (b!= 0) {
    currentState = StateDestroy;
    destroy();
    
  } else if (millis() - stateTimer < 300) {
    if (lastVisibleB & 0b00000111) { // + mid
      powStg = power40P;
      powDrt = powerMinForward;
      
    } else if (lastVisibleB & 0b01110000) {
      powStg = powerMinForward;
      powDrt = power40P;
    }
  } else {
    currentState = StateBackward;
    backward();
  }
}

void backward() {
  if (b != 0) {
    currentState = StateDestroy;
    destroy();
    
  } else if ((analogRead(whiteSensorStg) <= whiteThreshold) ||
  (analogRead(whiteSensorDrt) <= whiteThreshold)) { // both sensors show(ed) white
    currentState = StateForward;
    stateTimer = millis();
    forwardFullSpeed();
        
  } else {
    powStg = -power50P;
    powDrt = -power80P;
  }
}

