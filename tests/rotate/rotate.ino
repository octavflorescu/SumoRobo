#include "Utils.h"
#include "Config.h"

#define startModule 11//pin number for start module
int const whiteSensorStg = A1;
int const whiteSensorDrt = A0;

int powStg, powDrt;
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
  
  while (isLow(startModule)) {
    WriteMotors(0, 0);
  }
  stateTimer = millis();
}

void loop() {
    WriteMotors(power, -power);
}

