#ifndef Definitions_H
#define Definitions_H

#include "Utils.h"
#include "Config.h"

typedef enum state {
    StateGetOnTrack,
    StateRotate,
    StateAttack,
    StateForward,
    StateBackward,
    StateSearch,
    StateRotateSideBack,
    StateRotateSideFront,
    StateRotateFrontSide,
    StateHalt,
    StateFallbackSide
} State;

extern unsigned int power; // out of 255
extern unsigned int power90P;
extern unsigned int power85P;
extern unsigned int power80P;
extern unsigned int power75P;
extern unsigned int power70P;
extern unsigned int power65P;
extern unsigned int power60P;
extern unsigned int power55P;
extern unsigned int power50P;
extern unsigned int power40P;
extern unsigned int power35P;
extern unsigned int power30P;
extern unsigned int power25P;
extern unsigned int power20P;
extern unsigned int power15P;
extern unsigned int powerMin;

extern bool directionDrt;

extern int whiteSensorStg;
extern int whiteSensorDrt;
extern bool whiteSensorDrtSaw;
extern bool whiteSensorStgSaw;
extern int switchPin;
extern unsigned int power;

extern int powStg, powDrt;
extern long stateTimer;
extern State currentState;
extern short b;

//for Oracle
extern short lastVisibleB;

//for Saca
extern short forwardCounts;
extern short seeingForwardCounts;

//for Rotate
extern long backTimer;
extern long runTime;

void setupPowers(unsigned int putere);
bool checkForward();

#endif

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

//RotateStates
//typedef enum state {
//  StateAttack,
//  StatePause,
//  StateRotateStgBack, // lateral spate
//  StateRotateStgFront, // lateral fata
//  StateRotateFrontStg,
//  StateRotateDrtBack, // lateral spate
//  StateRotateDrtFront, // lateral fata
//  StateRotateFrontDrt,
//  StateRun --> get on track
//} State;

//TriangState
//typedef enum state {
//  stateForward,
//  stateTurnLeft
//} State;
