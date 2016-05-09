#include "Utils.h"
#include "Config.h"

int powStg, powDrt;
long stateTimer;
bool didSeeRight, didSeeLeft;
State currentState;

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

  WriteMotors(0, 0);
  delay(50);
  WriteMotors(0, 0);
  delay(50);

//  currentState = StateForward;
//  powStg = power;
//  powDrt = power;

  currentState = StateBackward;
  powStg = -power;
  powDrt = -power;
  
  while (isLow(startModule)) {
    WriteMotors(0, 0);
  }
}

void loop() {
  switch (currentState) {
    case StateForward:
      forward();
    break;

    case StateBackward:
      backward();
    break;
    
    default:
    break;
  }
  WriteMotors(powStg, powDrt);
}

//

void forward() {
  //Front sensors are taken into consideration
  didSeeRight = analogRead(whiteSensorDrtF) < kWhiteThreshold;
  didSeeLeft = analogRead(whiteSensorStgF) < kWhiteThreshold;

  if (didSeeLeft || didSeeRight) {
//    powStg = 0;
//    powDrt = 0;
//    currentState = StateGeneral;
    powStg = -power;
    powDrt = -power;
    currentState = StateBackward;
  }
}

void backward() {
  //Back(Spate) sensors are taken into consideration
  didSeeRight = analogRead(whiteSensorDrtS) < kWhiteThreshold;
  didSeeLeft = analogRead(whiteSensorStgS) < kWhiteThreshold;

  if (didSeeLeft || didSeeRight) {
    powStg = 0;
    powDrt = 0;
    currentState = StateGeneral;
//    powStg = power;
//    powDrt = power;
//    currentState = StateForward;
  }
}

