#include "Utils.h"
#include "Config.h"

int powStg, powDrt;
long stateTimer;
long timeInState;
State currentState;
bool didSeeRight, didSeeLeft;
bool shouldRotateLeft;

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
    sRawPacket_mds.sMotorCommand.pMotorR_sds16 = m1; \
    sRawPacket_mds.sMotorCommand.pMotorL_sds16 = -m2; \
    Serial1.write(sRawPacket_mds.data, BUFFER_LENGTH); \
  })

void setup() {
  Serial1.begin(57600);
  
  pinAsInput(startModule);

  powStg = power;
  powDrt = power;
  currentState = StateAttack;
  
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
      
    case StateRotate:
      rotate();
      break;

    default:
      break;
  }
  WriteMotors(powStg, powDrt);
}

////////////////////////////////
// Private
////////////////////////////////

void attack() {
  //Front sensors are taken into consideration
  didSeeRight = analogRead(whiteSensorDrtF) < kWhiteThreshold;
  didSeeLeft = analogRead(whiteSensorStgF) < kWhiteThreshold;

  if (didSeeLeft || didSeeRight) {
    if (didSeeLeft && didSeeRight) {
      shouldRotateLeft = random(2) == 1;
    } else {
      shouldRotateLeft = didSeeLeft;
    }
    if (shouldRotateLeft) {
      powStg = -power;
      powDrt = power75P;
      
    } else {
      powDrt = -power;
      powStg = power75P;
    }
    currentState = StateRotate;
    timeInState = 100;//random(160, 200);
    stateTimer = millis();
  }
}

void rotate() {
  if (millis() - stateTimer < timeInState) {
    //Back(Spate) sensors are taken into consideration
    didSeeRight = analogRead(whiteSensorDrtS) < kWhiteThreshold;
    didSeeLeft = analogRead(whiteSensorStgS) < kWhiteThreshold;

    if (didSeeLeft || didSeeRight) {
      powDrt = power;
      powStg = power;
      currentState = StateAttack;
    }
  } else {
    powDrt = power;
    powStg = power;
    currentState = StateAttack;
  }
}

