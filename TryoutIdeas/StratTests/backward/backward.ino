#include "Utils.h"
#include "Config.h"

//#define DEBUG

#define startModule 11//pin number for start module
int const whiteSensorStg = A1;
int const whiteSensorDrt = A0;

typedef enum state {
  StateForward,
  StateRotate,
  StateBackward,
  StateDestroy
} State;

long timer;
bool start;
bool rotating;
byte b;
int powStg, powDrt;
bool didHitLine;
long timerStart; // start stop

State currentState;
long stateTimer;

#define BUFFER_LENGTH 9

typedef struct 
{
  uint8_t packetStart[3];
  int16_t pMotorR_sds16;
  int16_t pMotorL_sds16;
  int16_t yCheck_sds16;
} motorCommand;

typedef union
{
  motorCommand sMotorCommand;
  uint8_t data[BUFFER_LENGTH];
} rawPacket;

#define WriteMotors(m1, m2) \
  ({ \
    rawPacket sRawPacket_mds = { '#', '#', '#', 0, 0, 0 }; \
    sRawPacket_mds.sMotorCommand.pMotorR_sds16 = (int16_t)(m1 * 0.925); \
    sRawPacket_mds.sMotorCommand.pMotorL_sds16 = -m2; \
    Serial1.write(sRawPacket_mds.data, BUFFER_LENGTH); \
  })

void setup() {
  // SETUP
  start = true;
  rotating = false;
  Serial1.begin(57600);
  powStg = 0;
  powDrt = 0;
  // READ FROM SENSORS
  DDRB = 0b00000000;
  didHitLine = false;

  // START MODULE
  pinAsInput(startModule);
  WriteMotors(0, 0);
  delay(100);
  
#ifdef DEBUG
  timerStart = millis();
#endif
  
  while (isLow(startModule)) {
  // DO NOT START UNTIL START/STOP MODULE: STARTED
  // ALL OTHER INIT CODE APART FROM TIMER SHOULD BE ABOVE THIS
#ifdef DEBUG
    if (millis() > timerStart + 5000) {
      break;
    }
#endif
    WriteMotors(0, 0);
  }
}

void loop() {
  // START PASSED, GO TO SENSOR CHECK
//  b = PINB & 0b01111111;
  if (!didHitLine) {
    backward(); 
  } else if (millis() - timerStart > 225) {
    powStg = 0;
    powDrt = 0;
  }
  WriteMotors(powStg, powDrt);
}

bool destroy() {
  if (b == 0) {
      currentState = StateBackward;
      backward();
  } else {
    if (b & 0b00011100) {
      if (b & 0b00011100 == 0b00011100){
        powStg = power;
        powDrt = power;
      } else if (b & 0b00011000 == 0b00011000){
        powStg = power90P;
        powDrt = power;
      } else if (b & 0b00001100 == 0b00001100){
        powStg = power;
        powDrt = power90P;
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
        powStg = -power25P;
        powDrt = power20P;
      } else {
        powStg = -power30P;
        powDrt = power15P;
      }
    } else {
      if (b & 0b00000010) {
        powStg = power20P;
        powDrt = -power25P;
      } else {
        powStg = power15P;
        powDrt = -power30P;
      }
    }
  }
}

void forwardFullSpeed() {
  if (b != 0) {
    currentState = StateDestroy;
    destroy();
    
  } else if (millis() - stateTimer < 225) {
      powStg = power75P;
      powDrt = power;
      
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
      powDrt = power75P;
    }
}

bool doesWhiteStgSee;
bool doesWhiteDrtSee;

void backward() {
  doesWhiteStgSee = (analogRead(whiteSensorStg) <= whiteThreshold);
  doesWhiteDrtSee = (analogRead(whiteSensorDrt) <= whiteThreshold);
  
  if (doesWhiteStgSee || doesWhiteDrtSee) {
//    if (doesWhiteStgSee && doesWhiteDrtSee) { // both sensors show(ed) white
      didHitLine = true;
      powStg = power;
      powDrt = power;
      timerStart = millis();
//    } else if (doesWhiteStgSee) {
//      powStg = 0;
//      powDrt = -power35P;
//      
//    } else {
//      powStg = -power35P;
//      powDrt = 0;
//    }
  } else {
    powStg = -power90P;
    powDrt = -power90P;
  }
}

