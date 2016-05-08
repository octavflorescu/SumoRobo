#include "MotorCommands.h"

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <Arduino.h>

#define power 255
#define zeroTo100 70

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#define BUFFER_LENGTH 9

typedef struct {
    uint8_t packetStart[3];
    int16_t pMotorR_sds16;
    int16_t pMotorL_sds16;
    int16_t yCheck_sds16;
} motorCommand;

typedef union {
    motorCommand sMotorCommand;
    uint8_t data[BUFFER_LENGTH];
} rawPacket;

//(int16_t)(m1 * 0.925);

#define WriteMotors(m1, m2) \
({ \
rawPacket sRawPacket_mds = { '#', '#', '#', 0, 0, 0 }; \
sRawPacket_mds.sMotorCommand.pMotorR_sds16 = (int16_t)(m1 * power); \
sRawPacket_mds.sMotorCommand.pMotorL_sds16 = -(int16_t)(m2 * power); \
Serial.write(sRawPacket_mds.data, BUFFER_LENGTH); \
})

/////////////////////////////////////////////////////////////////////
//////////////////// Variable Declarations //////////////////////////

bool direct = false;

typedef struct {
    int stg;
    int drt;
} motorPower;

typedef struct {
    long t0;//time at calculation
    long stg;
    long drt;
} motorDeltaTime;

motorPower currentMotorPower = {0, 0};
motorPower targetMotorPower = {0, 0};
motorPower initialMotorPower = {0, 0};
motorPower powerSign = {1, 1};

motorDeltaTime timeNeeded = {LONG_MAX, LONG_MAX, LONG_MAX};
motorDeltaTime variableTime = {LONG_MAX, 0, 0};
long passedTime = 0;

/////////////////////////////////////////////////////////////////////
//////////////////////////// Private ////////////////////////////////

motorPower currentPower() {
    powerSign = {targetMotorPower.stg >= 0 ? 1 : -1, targetMotorPower.drt >= 0 ? 1 : -1};
    
    if (direct) {
        passedTime = min(micros() - timeNeeded.t0, 0);
        
        currentMotorPower.stg = int(initialMotorPower.stg + abs(initialMotorPower.stg - targetMotorPower.stg) * min(passedTime / timeNeeded.stg, 1) * powerSign.stg);
        currentMotorPower.drt = int(initialMotorPower.drt + abs(initialMotorPower.drt - targetMotorPower.drt) * min(passedTime / timeNeeded.drt, 1) * powerSign.drt);
        
    } else {
        passedTime = max(timeNeeded.stg, timeNeeded.drt);
        
        currentMotorPower.stg = int(currentMotorPower.stg + abs(currentMotorPower.stg - targetMotorPower.stg) / passedTime * powerSign.stg);
        currentMotorPower.drt = int(currentMotorPower.drt + abs(currentMotorPower.drt - targetMotorPower.drt) / passedTime * powerSign.drt);
    }
    return currentMotorPower;
}

motorDeltaTime deltaTime() {
    variableTime.t0 = micros();
    variableTime.stg = long(abs(currentMotorPower.stg - targetMotorPower.stg) * zeroTo100);
    variableTime.drt = long(abs(currentMotorPower.drt - targetMotorPower.drt) * zeroTo100);
    
    return variableTime;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////// Functions ///////////////////////////////

void writeMotors(int stg, int drt) {
    if (!direct) {
        initialMotorPower = currentPower();
    }
    
    direct = true; // motor command state
    
    targetMotorPower.stg = stg;
    targetMotorPower.drt = drt;
    
    timeNeeded = deltaTime();
    initialMotorPower = currentPower();
    
    WriteMotors(stg, drt);
}

void controlledWriteMotors(int stg, int drt) {
    targetMotorPower.stg = stg;
    targetMotorPower.drt = drt;
    
    if (direct) {
        currentMotorPower = currentPower();
    }
    
    timeNeeded = deltaTime();
    
    direct = false; // motor command state
    currentMotorPower = currentPower();
    
    WriteMotors(currentMotorPower.stg, currentMotorPower.drt);
}
