#ifndef CONFIG_H
#define CONFIG_H

typedef enum state {
  StateAttack,
  StateRotate
} State;

#define power     255 // out of 255
#define power95P (int16_t)(power * 0.95)
#define power90P (int16_t)(power * 0.9)
#define power80P (int16_t)(power * 0.8)
#define power75P (int16_t)(power * 0.75)
#define power70P (int16_t)(power * 0.7)
#define power60P (int16_t)(power * 0.6)
#define power50P (int16_t)(power * 0.5)
#define power35P (int16_t)(power * 0.35)
#define power30P (int16_t)(power * 0.3)
#define power25P (int16_t)(power * 0.25)
#define power20P (int16_t)(power * 0.2)
#define power15P (int16_t)(power * 0.15)

#define kWhiteThreshold 70 // max white

#define startModule 12//pin number for start module
int const whiteSensorStgF = A0;
int const whiteSensorDrtF = A1;
int const whiteSensorStgS = A2;
int const whiteSensorDrtS = A3;

#endif
