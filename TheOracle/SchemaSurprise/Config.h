#ifndef CONFIG_H
#define CONFIG_H

#define rotateSTG
//#define rotateDRT

#define power     255 //out of 255
#define power90P (int16_t)(power * 0.9)
#define power85P (int16_t)(power * 0.85)
#define power80P (int16_t)(power * 0.8)
#define power75P (int16_t)(power * 0.75)
#define power70P (int16_t)(power * 0.7)
#define power60P (int16_t)(power * 0.6)
#define power50P (int16_t)(power * 0.5)
#define power40P (int16_t)(power * 0.4)
#define power35P (int16_t)(power * 0.35)
#define power30P (int16_t)(power * 0.3)
#define power25P (int16_t)(power * 0.25)
#define power20P (int16_t)(power * 0.2)
#define power15P (int16_t)(power * 0.15)
#define minPower 20

#define whiteThreshold 120

typedef struct 
{
  uint8_t packetStart[3];
  int16_t pMotorR_sds16;
  int16_t pMotorL_sds16;
  int16_t yCheck_sds16;
} motorCommand;

#define BUFFER_LENGTH 9

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

#define startModule 11//pin number for start module

int const whiteSensorStg = A1;
int const whiteSensorDrt = A0;

#endif
