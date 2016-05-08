#include "Utils.h"
#include "Config.h"

#define startModule 11//pin number for start module
int const whiteSensorStg = A1;
int const whiteSensorDrt = A0;

typedef enum state {
  StateSearch,
  StateAiming,
  StateAttack
} State;

byte b;
byte searchB;
int powStg, powDrt;
State currentState;
long timerB;

void setup() {
  // SETUP
  Serial1.begin(57600);
  powStg = 0;
  powDrt = 0;
  // READ FROM SENSORS
  DDRB = 0b00000000;
  
  // START MODULE
  pinAsInput(startModule);
  WriteMotors(0, 0);
  delay(100);
  
  while (isLow(startModule)) {
    WriteMotors(0, 0);
  }
  b = 0;
  searchB = 0;
  currentState = StateSearch;
}

void loop() {
  b = PINB & 0b01111111;
  searchB = PINB & 0b01110111 ? : searchB;
  
  if (currentState == StateAttack) {
    attack();
  } else if (currentState == StateAiming) {
    aim();
  } else { // StateSearching
    search();
  }
  WriteMotors(powStg, powDrt);
}

void attack() {
  if (b == 0) {
    currentState = StateSearch;
    search();
    
  } else {
      powStg = power;
      powDrt = power;
  }
}

void aim() {
  if (b & 0b00001000){
    if (millis() - timerB > 3000) {
      currentState = StateAttack;
      attack();
    }
  } else {
    currentState = StateSearch;
    search();
  }
}

void search() {
  if (b & 0b00001000 == 0b00001000) {
    timerB = millis();
    currentState = StateAiming;
    searchB = b & 0b01110111 ? : searchB;
    powStg = 0;
    powDrt = 0;
    
  } else {
    if (b & 0b00000111) {
      powStg = power50P;
      powDrt = -minPower;
      
    } else if (b & 0b01110000) {
      powStg = -minPower;
      powDrt = power50P;
      
    } else if (searchB & 0b00000111) { // + mid
      powStg = power60P;
      powDrt = -minPower;
      
    } else { // if (lastVisibleB & 0b01110000)
      powStg = -minPower;
      powDrt = power60P;
    }
  }
}

