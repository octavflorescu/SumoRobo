#include "Declarations.h"

unsigned int power = 255; // out of 255
unsigned int power90P;
unsigned int power85P;
unsigned int power80P;
unsigned int power75P;
unsigned int power70P;
unsigned int power65P;
unsigned int power60P;
unsigned int power55P;
unsigned int power50P;
unsigned int power40P;
unsigned int power35P;
unsigned int power30P;
unsigned int power25P;
unsigned int power20P;
unsigned int power15P;
unsigned int powerMin = 5;

bool directionDrt;

int whiteSensorStg;
int whiteSensorDrt;
bool whiteSensorDrtSaw;
bool whiteSensorStgSaw;
int switchPin;

int powStg, powDrt;
long stateTimer;
State currentState;
short b;

//for Oracle
short lastVisibleB;

//for Saca
short forwardCounts;
short seeingForwardCounts;

//for Rotate
long backTimer;
long runTime;

void setupPowers(unsigned int putere) {
power = putere;
power90P  = (int16_t)(power * 0.9);
power85P  = (int16_t)(power * 0.85);
power80P  = (int16_t)(power * 0.8);
power75P  = (int16_t)(power * 0.75);
power70P  = (int16_t)(power * 0.7);
power65P  = (int16_t)(power * 0.65);
power60P  = (int16_t)(power * 0.6);
power55P  = (int16_t)(power * 0.55);
power50P  = (int16_t)(power * 0.5);
power40P  = (int16_t)(power * 0.4);
power35P  = (int16_t)(power * 0.35);
power30P  = (int16_t)(power * 0.3);
power25P  = (int16_t)(power * 0.25);
power20P  = (int16_t)(power * 0.2);
power15P  = (int16_t)(power * 0.15);
}

bool checkForward() {
  if (b & 0b00001000 && millis() - stateTimer > 360) {
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
        
        return true;
        
      } else if (b == 0b00011000) {
        currentState = StateFallbackSide;
        powStg = -power50P;
        powDrt = power;
        stateTimer = millis();
        
        return true;
        
      } else if (b == 0b00001100) {
        currentState = StateFallbackSide;
        powDrt = -power50P;
        powStg = power;
        stateTimer = millis();
        
        return true;
        
      } else { // b == mid only
        // extend the time
        stateTimer = millis() - 275;
      }
    }
    return false;
}

//OracleStates
//typedef enum state {
//    StateForward,
//    StateGetOnTrack,
//    StateRotate,
//    StateBackward,
//    StateDestroy,
//    StateJustSearch
//} State;

//SacaStates
//typedef enum state {
//    StateAttack,
//    StateForward,
//    StateSearch,
//    StateRotateSideBack,
//    StateRotateSideFront,
//    StateRotateFrontSide,
//    StateHalt
//} State;
